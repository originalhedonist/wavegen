#include "stdafx.h"
#include "channel.h"

void write_short(std::ostream& os, int16_t i);

void write_wav(std::ostream& ofs, std::vector<channel>& channels, const headerdata& h)
{
    char tempfilename[FILENAME_MAX];
    tmpnam_s(tempfilename, FILENAME_MAX);
    std::ofstream ofstemp;
    ofstemp.open(tempfilename, std::ios::trunc | std::ios::binary);
    if (!ofstemp.is_open()) throw std::exception("Could not open temporary file");

    double* maxPerChannel = new double[channels.size()];
    memset(maxPerChannel, 0, sizeof(double) * channels.size());

    for (int n = 0; n < h.N; n++)
    {
        double t = (double)h.length_seconds * (double)n / h.N;
        int channelIndex = 0;

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

            if (a < -1 || a > 1) throw std::exception("Amplitude must be -1 to 1");

            double newmax = std::max(maxPerChannel[channelIndex], abs(a));
            maxPerChannel[channelIndex] = newmax;
            channelIndex++;

            ofstemp.write(reinterpret_cast<char*>(&a), sizeof(a));
        }
    }

    ofstemp.close();
    std::ifstream ifstemp;
    ifstemp.open(tempfilename, std::ios::binary);
    if (!ifstemp.is_open()) throw std::exception("Could not open temp file");

    int16_t maxshort = 0, minshort = 0;

    for (int n = 0; n < h.N; n++)
    {
        for (int c = 0; c < channels.size(); c++)
        {
            double a;
            ifstemp.read(reinterpret_cast<char*>(&a), sizeof(a));
            a /= maxPerChannel[c]; //normalization
            int aL = ((a + 1) * ((double)65535 / 2) - 32768);
            if (aL > SHRT_MAX) throw std::exception("Exceeded SHRT_MAX");
            if (aL < SHRT_MIN) throw std::exception("Exceeded SHRT_MIN");
            int16_t aLs = (int16_t)aL;
            write_short(ofs, aLs);
        }
    }
    ifstemp.close();
    remove(tempfilename);
    delete[] maxPerChannel;
}