using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DTMF2
{
    public class SineWaveProvider32 : WaveProvider32
    {
        double[] samples;

        public SineWaveProvider32()
        {
        }

        public double[] Samples 
        {
            get
            {
                return samples;
            }
            set
            {
                samples = value;
            }
        }

        public override int Read(float[] buffer, int offset, int sampleCount)
        {
            byte[] array = new byte[samples.Length * 8];
            Buffer.BlockCopy(samples, 0, array, 0, array.Length);
            int sampleRate = WaveFormat.SampleRate;
            for (int n = 0; n < samples.Length; n++)
            {
                buffer[n + offset] = (float)(array[n + offset]);
            }
            return array.Length;
        }
    }
}
