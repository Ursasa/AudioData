#include "Dtmf.h"


Dtmf::Dtmf(void)
{
	SampleRate = 8000;
	goertzel_freq_length = 8;
	goertzel_freq = new double[goertzel_freq_length];
	goertzel_freq[0] = 1209.0;
	goertzel_freq[1] = 1336.0;
	goertzel_freq[2] = 1477.0;
	goertzel_freq[3] = 1633.0;
	goertzel_freq[4] = 697.0;
	goertzel_freq[5] = 770.0;
	goertzel_freq[6] = 852.0;
	goertzel_freq[7] = 941.0;
	iMilliSeconds = 100;
}


Dtmf::~Dtmf(void)
{
	delete[] m_pTone;
	delete[] goertzel_freq;
}

Dtmf::Dtmf(char digit)
{
	SampleRate = 8000;
	goertzel_freq_length = 8;
	goertzel_freq = new double[goertzel_freq_length];
	goertzel_freq[0] = 1209.0;
	goertzel_freq[1] = 1336.0;
	goertzel_freq[2] = 1477.0;
	goertzel_freq[3] = 1633.0;
	goertzel_freq[4] = 697.0;
	goertzel_freq[5] = 770.0;
	goertzel_freq[6] = 852.0;
	goertzel_freq[7] = 941.0;
	iMilliSeconds = 100;
	SetDigit(digit);
}

void Dtmf::SetDigit(char digit)
{
	m_digit = digit;
}

void Dtmf::Init()
{
	GenerateSample();
	for (int i=0; i<goertzel_freq_length; i++)
	{
		s_prev[goertzel_freq[i]] = 0;
		s_prev2[goertzel_freq[i]] = 0;
		totalpower[goertzel_freq[i]] = 0;
		N[goertzel_freq[i]] = 0;
		double normalizedfreq = goertzel_freq[i] / SampleRate;
		coeff[goertzel_freq[i]] = 2.0 * cos(2.0 * M_PI * normalizedfreq);
	}
}


void Dtmf::InitResultDictionary()
{
	Result[std::pair<double, double>(1209, 697)] = "1";
	Result[std::pair<double, double>(1336, 697)] = "2";
	Result[std::pair<double, double>(1477, 697)] = "3";
	Result[std::pair<double, double>(1633, 697)] = "A";
	Result[std::pair<double, double>(1209, 770)] = "4";
	Result[std::pair<double, double>(1336, 770)] = "5";
	Result[std::pair<double, double>(1477, 770)] = "6";
	Result[std::pair<double, double>(1633, 770)] = "B";
	Result[std::pair<double, double>(1209, 852)] = "7";
	Result[std::pair<double, double>(1336, 852)] = "8";
	Result[std::pair<double, double>(1477, 852)] = "9";
	Result[std::pair<double, double>(1633, 852)] = "C";
	Result[std::pair<double, double>(1209, 941)] = "*";
	Result[std::pair<double, double>(1336, 941)] = "0";
	Result[std::pair<double, double>(1477, 941)] = "#";
	Result[std::pair<double, double>(1633, 941)] = "D";
}


void Dtmf::GenerateSample()
{
	m_iPacketLength = iMilliSeconds * SampleRate / 1000;
	m_pTone = new double[m_iPacketLength];

	if (m_pTone == NULL)
	{
		return;
	}

	int lowtone_frequency = 0;
	int hightone_frequency = 0;

	switch (m_digit)
	{
		case '1':
		case '2':
		case '3':
		case 'A': lowtone_frequency = 697; break;
		case '4':
		case '5':
		case '6':
		case 'B': lowtone_frequency = 770; break;
		case '7':
		case '8':
		case '9':
		case 'C': lowtone_frequency = 852; break;
		case '*':
		case '0':
		case '#':
		case 'D': lowtone_frequency = 941; break;
	}
	switch (m_digit)
	{
		case '1':
		case '4':
		case '7':
		case '*': hightone_frequency = 1209; break;
		case '2':
		case '5':
		case '8':
		case '0': hightone_frequency = 1336; break;
		case '3':
		case '6':
		case '9':
		case '#': hightone_frequency = 1477; break;
		case 'A':
		case 'B':
		case 'C':
		case 'D': hightone_frequency = 1633; break;
	}

	double pi_prod_1 = (2.0 * M_PI * lowtone_frequency) / SampleRate;
	double pi_prod_2 = (2.0 * M_PI * hightone_frequency) / SampleRate;

	for (int i = 0; i < m_iPacketLength; i++)
	{
		m_pTone[i] = 128 + 63 * sin(i * pi_prod_1) + 63 * cos(i * pi_prod_2);
	}
}


double* Dtmf::GetData()
{
	double* result = new double[m_iPacketLength];
	for (int i=0; i<m_iPacketLength; i++)
		result[i] = m_pTone[i];
	return result;
}


int Dtmf::GetLength()
{
	return m_iPacketLength;
}


std::string Dtmf::Run(double sample)
{
	std::map<double, double> freqs;
	for (int i=0; i<goertzel_freq_length; i++)
	{
		double s = sample + (coeff[goertzel_freq[i]] * s_prev[goertzel_freq[i]]) - s_prev2[goertzel_freq[i]];
		s_prev2[goertzel_freq[i]] = s_prev[goertzel_freq[i]];
		s_prev[goertzel_freq[i]] = s;
		N[goertzel_freq[i]] += 1;
		double power = (s_prev2[goertzel_freq[i]] * s_prev2[goertzel_freq[i]]) + (s_prev[goertzel_freq[i]] * s_prev[goertzel_freq[i]]) - (coeff[goertzel_freq[i]] * s_prev[goertzel_freq[i]] * s_prev2[goertzel_freq[i]]);
		totalpower[goertzel_freq[i]] += sample * sample;
		if (totalpower[goertzel_freq[i]] == 0)
			totalpower[goertzel_freq[i]] = 1;
		freqs[goertzel_freq[i]] = power / totalpower[goertzel_freq[i]] / N[goertzel_freq[i]];
	}
	return GetNumber(freqs);
}


std::string Dtmf::GetNumber(std::map<double, double> freqs)
{
	double hi[] = {1209.0,1336.0,1477.0,1633.0};
	double lo[] = {697.0,770.0,852.0,941.0};

	double hifreq = 0.0;
	double hifreq_v = 0.0;
	for (int i=0; i<goertzel_freq_length / 2; i++)
	{
		if (freqs[hi[i]]>hifreq_v)
		{    
			hifreq_v = freqs[hi[i]];
			hifreq = hi[i];
		}
	}
	         
	double lofreq = 0.0;
	double lofreq_v = 0.0;
	for (int i=0; i<goertzel_freq_length / 2; i++)
	{
		if (freqs[lo[i]]>lofreq_v)
		{    
			lofreq_v = freqs[lo[i]];
			lofreq = lo[i];
		}
	}

	std::pair<double, double> pairKey(hifreq, lofreq);
	return Result[pairKey];
}
