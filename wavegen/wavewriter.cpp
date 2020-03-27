#include "stdafx.h"
#include "channel.h"

void write_short(std::ostream& os, int16_t i);

void write_wav(std::ostream& ofs, std::vector<channel>& channels, const headerdata& h)
{
    for (int n = 0; n < h.N; n++)
    {
        double t = (double)h.length_seconds * (double)n / h.N;

        for (std::vector<channel>::iterator channelit = channels.begin(); channelit != channels.end(); channelit++)
        {
            double a = 1;

            for (std::vector<FrequencyFunctionWaveFile>::iterator componentit = channelit->components.begin();
                componentit != channelit->components.end();
                componentit++)
            {
                auto athis = componentit->Amplitude(t, n);
                a *= athis;
            }
            if (a < -1 || a > 1)
                throw std::exception("Amplitude must be -1 to 1");

            int16_t aLs = (int16_t)((a + 1) * ((double)65535 / 2) - 32768);
            write_short(ofs, aLs);
        }
    }
}