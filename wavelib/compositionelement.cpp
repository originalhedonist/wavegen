#include "pch.h"
#include "compositionelement.h"
#include "wavfuncs.h"
using json = nlohmann::json;

compositionelement::compositionelement(const nlohmann::json& j)
    : compositionelement(j, (int16_t)j["Channels"].size()) {}

compositionelement::compositionelement(const nlohmann::json& j, const int16_t numChannels)
    : header(trackLength(j), numChannels)
{
    for (auto channeljson : j["Channels"])
    {
        channels.push_back(channel(channeljson, header));
    }
    maxPerChannel = new double[channels.size()];
    memset(maxPerChannel, 0, sizeof(double) * channels.size());
    memset(tempfilename, 0, FILENAME_MAX);
}

int32_t compositionelement::trackLength(const nlohmann::json& j)
{
    std::string track_length_string = j["TrackLength"];
    int32_t track_length = wavfuncs::time_span_to_seconds(track_length_string);
    return track_length;
}

compositionelement::~compositionelement()
{
    delete[] maxPerChannel;
}

double compositionelement::get_next(int n, int channel)
{
    if (n != nNext) throw std::exception("Called get_next with n out of sequence");
    if (channel != channelNext) throw std::exception("Called get_next with channel out of sequence");
    double a;
    ifstemp.read(reinterpret_cast<char*>(&a), sizeof(a));
    double aNorm = a / maxPerChannel[channel]; //normalization
    if (++channelNext >= channels.size())
    {
        nNext++;
        channelNext = 0;
    }
    return aNorm;
}

bool compositionelement::is_complete() const
{
    return nNext >= header.N;
}

void compositionelement::calculate()
{
    tmpnam_s(tempfilename, FILENAME_MAX);
    std::ofstream ofstemp;
    ofstemp.open(tempfilename, std::ios::trunc | std::ios::binary);
    if (!ofstemp.is_open()) throw std::exception("Could not open temporary file");

    for (int n = 0; n < header.N; n++)
    {
        double t = (double)header.length_seconds * (double)n / header.N;
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
}

void compositionelement::start()
{
    ifstemp.open(tempfilename, std::ios::binary);
    if (!ifstemp.is_open()) throw std::exception("Could not read temp file");
}

void compositionelement::stop()
{
    ifstemp.close();
}

