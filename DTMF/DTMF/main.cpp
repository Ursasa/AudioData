#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

#define M_PI       3.14159265358979323846
#define SAMPLERATE 8000
double goertzel_freq[] = { 1209.0,1336.0,1477.0,1633.0,697.0,770.0,852.0,941.0 };

class DTMF 
{
public:
	DTMF(char digit, int iMilliSeconds = 100, WORD wSampleRate = 8000) 
	{
		m_iPacketLength = iMilliSeconds * 8000/1000;
		m_pTone = new BYTE[m_iPacketLength];

		if(m_pTone == NULL){
			return;
		}

		int lowtone_frequency = 0;
		int hightone_frequency = 0;

		switch(digit)
		{
		case '1': case '2': case '3': case 'A': lowtone_frequency =  697; break;
		case '4': case '5': case '6': case 'B': lowtone_frequency =  770; break;
		case '7': case '8': case '9': case 'C': lowtone_frequency =  852; break;
		case '*': case '0': case '#': case 'D': lowtone_frequency =  941; break;
		}
		switch(digit)
		{
		case '1': case '4': case '7': case '*': hightone_frequency =  1209; break;
		case '2': case '5': case '8': case '0': hightone_frequency =  1336; break;
		case '3': case '6': case '9': case '#': hightone_frequency =  1477; break;
		case 'A': case 'B': case 'C': case 'D': hightone_frequency =  1633; break;
		}

		double pi_prod_1 = (2.0 * M_PI * lowtone_frequency)/wSampleRate;
		double pi_prod_2 = (2.0 * M_PI * hightone_frequency)/wSampleRate;

		for(int i=0; i<m_iPacketLength; i++)
		{
			m_pTone[i] = 128 + BYTE(63*sin(i*pi_prod_1) + 63*sin(i*pi_prod_2));
		}
	}

	~DTMF() {
		if(m_pTone != NULL){
			delete[] m_pTone;
			m_pTone = NULL;
		}
	}
public:
	PBYTE GetData() const 
	{
		return m_pTone;
	}

	int GetLength() const 
	{
		return m_iPacketLength;
	}
private:
	PBYTE m_pTone;
	int m_iPacketLength;
};

double CalculateGoertzel(PBYTE sample, int length, double frequency, int samplerate) 
{ 
	double Skn, Skn1, Skn2; 
	Skn = Skn1 = Skn2 = 0; 
	for (int i = 0; i < length; i++) 
	{ 
		Skn2 = Skn1; 
		Skn1 = Skn; 
		Skn = 2 * cos(2 * M_PI * frequency / samplerate) * Skn1 - Skn2 + sample[i]; 
	}

	double WNk = exp(-2 * M_PI * frequency / samplerate);

	return 20* log10(abs((Skn - WNk * Skn1))); 

	/*double k = 0.5 + (float)length * frequency / samplerate;
	double omega = (2.0 * M_PI * k) / (float)length;
	double sine = sin(omega);
	double cosine = cos(omega);
	double coeff = 2.0 * cosine;

	double real = (Skn - Skn1 * cosine);
	double imag = (Skn1 * sine);

	double magnitudeSquared = real*real + imag*imag;
	double magnitude = sqrt(magnitudeSquared);
	return magnitude;*/
}

void main()
{
	char digit = '1';
	DTMF dtmf(digit);
	cout << (int)digit << endl;
	
}