#include "pch.h"
#include "channel.h"
#include "compositionelement.h"

void write_short(std::ostream& os, int16_t i);

void write_wav(std::ostream& ofs, const headerdata& hOverall, int channels, compositionelement& ce)
{
    for (int n = 0; n < hOverall.N; n++)
    {
        for (int c = 0; c < channels; c++)
        {
            double a = ce.get_next(n, c);
            int aL = ((a + 1) * ((double)65535 / 2) - 32768);
            if (aL > SHRT_MAX) throw std::exception("Exceeded SHRT_MAX");
            if (aL < SHRT_MIN) throw std::exception("Exceeded SHRT_MIN");
            int16_t aLs = (int16_t)aL;
            write_short(ofs, aLs);
        }
    }
}