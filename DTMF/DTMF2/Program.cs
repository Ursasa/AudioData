using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DTMF2
{
    class Program
    {       
        static void Main(string[] args)
        {
            DTMF dtmf = new DTMF();
            dtmf.InitResultDictionary();

            dtmf.SetDigit('6');
            dtmf.Init();
            double[] test = dtmf.GetData();
            for (int i = 0; i < test.Length; i++)
            {
                Console.Write(dtmf.Run(test[i]) + " ");
            }
            Console.WriteLine();

            //dtmf.SetDigit('9');
            //dtmf.Init();
            //test = dtmf.GetData();
            //for (int i = 0; i < test.Length; i++)
            //{
            //    Console.Write(dtmf.Run(test[i]) + " ");
            //}
            //Console.WriteLine();

            
        }
    }

    public class DTMF
    {
        private class PairKey
        {
            public double High_Fre { get; set; }
            public double Low_Fre { get; set; }

            public override bool Equals(object obj)
            {
                return Equals(obj as PairKey);
            }

            public bool Equals(PairKey other)
            {
                return High_Fre == other.High_Fre && Low_Fre == other.Low_Fre;
            }

            public override int GetHashCode()
            {
                return High_Fre.GetHashCode() + Low_Fre.GetHashCode();
            }
        }

        private static double SampleRate = 8000;
        private double[] m_pTone;
        private int m_iPacketLength;
        private double[] goertzel_freq = { 1209.0, 1336.0, 1477.0, 1633.0, 697.0, 770.0, 852.0, 941.0 };
        private Dictionary<double, double> s_prev, s_prev2, totalpower, N, coeff;
        private Dictionary<PairKey, string> Result = new Dictionary<PairKey, string>();
        private char m_digit;
        private int iMilliSeconds = 100;

        public DTMF() { }

        public DTMF(char digit)
        {
            SetDigit(digit);
        }

        public void SetDigit(char digit)
        {
            m_digit = digit;
        }

        public void Init()
        {
            GenerateSample();
            s_prev = new Dictionary<double, double>();
            s_prev2 = new Dictionary<double, double>();
            totalpower = new Dictionary<double, double>();
            N = new Dictionary<double, double>();
            coeff = new Dictionary<double, double>();
            foreach (double k in goertzel_freq)
            {
                s_prev[k] = 0;
                s_prev2[k] = 0;
                totalpower[k] = 0;
                N[k] = 0;
                double normalizedfreq = k / SampleRate;
                coeff[k] = 2.0 * Math.Cos(2.0 * Math.PI * normalizedfreq);
            }
        }

        public void InitResultDictionary()
        {
            Result.Add(new PairKey { High_Fre = 1209, Low_Fre = 697 }, "1");
            Result.Add(new PairKey { High_Fre = 1336, Low_Fre = 697 }, "2");
            Result.Add(new PairKey { High_Fre = 1477, Low_Fre = 697 }, "3");
            Result.Add(new PairKey { High_Fre = 1633, Low_Fre = 697 }, "A");
            Result.Add(new PairKey { High_Fre = 1209, Low_Fre = 770 }, "4");
            Result.Add(new PairKey { High_Fre = 1336, Low_Fre = 770 }, "5");
            Result.Add(new PairKey { High_Fre = 1477, Low_Fre = 770 }, "6");
            Result.Add(new PairKey { High_Fre = 1633, Low_Fre = 770 }, "B");
            Result.Add(new PairKey { High_Fre = 1209, Low_Fre = 852 }, "7");
            Result.Add(new PairKey { High_Fre = 1336, Low_Fre = 852 }, "8");
            Result.Add(new PairKey { High_Fre = 1477, Low_Fre = 852 }, "9");
            Result.Add(new PairKey { High_Fre = 1633, Low_Fre = 852 }, "C");
            Result.Add(new PairKey { High_Fre = 1209, Low_Fre = 941 }, "*");
            Result.Add(new PairKey { High_Fre = 1336, Low_Fre = 941 }, "0");
            Result.Add(new PairKey { High_Fre = 1477, Low_Fre = 941 }, "#");
            Result.Add(new PairKey { High_Fre = 1633, Low_Fre = 941 }, "D");
        }

        public void GenerateSample()
        {
            m_iPacketLength = iMilliSeconds * 8000 / 1000;
            m_pTone = new double[m_iPacketLength];

            if (m_pTone == null)
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

            double pi_prod_1 = (2.0 * Math.PI * lowtone_frequency) / SampleRate;
            double pi_prod_2 = (2.0 * Math.PI * hightone_frequency) / SampleRate;

            for (int i = 0; i < m_iPacketLength; i++)
            {
                m_pTone[i] = 128 + 63 * Math.Sin(i * pi_prod_1) + 63 * Math.Sin(i * pi_prod_2);
            }
        }

        public double[] GetData() 
	    {
		    return m_pTone;
	    }

	    public int GetLength() 
	    {
		    return m_iPacketLength;
	    }       

        public string Run(double sample)
        {
            Dictionary<double, double> freqs = new Dictionary<double, double>();
		    foreach (double freq in goertzel_freq)
		    {	
			    double s = sample + (coeff[freq] * s_prev[freq]) - s_prev2[freq];
			    s_prev2[freq] = s_prev[freq];
			    s_prev[freq] = s;
                N[freq] += 1;
                double power = (s_prev2[freq] * s_prev2[freq]) + (s_prev[freq] * s_prev[freq]) - (coeff[freq] * s_prev[freq] * s_prev2[freq]);
                totalpower[freq] += sample * sample;
                if (totalpower[freq] == 0)
                    totalpower[freq] = 1;
                freqs[freq] = power / totalpower[freq] / N[freq];
		    }
            return GetNumber(freqs);
        }

        public string GetNumber(Dictionary<double, double> freqs)
        {
            double[] hi = {1209.0,1336.0,1477.0,1633.0};
            double[] lo = {697.0,770.0,852.0,941.0};
            double hifreq = 0.0;
            double hifreq_v = 0.0;
            foreach (double f in hi)
            {
                if (freqs[f]>hifreq_v)
                {    
                    hifreq_v = freqs[f];
                    hifreq = f;
                }
            }          
            double lofreq = 0.0;
            double lofreq_v = 0.0;
            foreach (double f in lo)
            {
                if (freqs[f]>lofreq_v)
                {    
                    lofreq_v = freqs[f];
                    lofreq = f;
                }
            }
            PairKey pairKey = new PairKey { High_Fre = hifreq, Low_Fre = lofreq };
            return Result[pairKey];
        }
    }
}
