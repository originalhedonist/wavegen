#include "pch.h"
#include "compositionelement.h"
#include "wavfuncs.h"
using json = nlohmann::json;

compositionelement::compositionelement(const nlohmann::json &j, const std::map<std::string, double> &constants)
    : compositionelement(j, constants, (int16_t)j["Channels"].size()) {}

compositionelement::compositionelement(const nlohmann::json &j, const std::map<std::string, double> &constants, const int16_t numChannels)
    : header(trackLength(j), numChannels), constants(constants)
{
    int channelindex = 0;
    for (auto channeljson : j["Channels"])
    {

        channels.push_back(channel(channeljson, constants, (double)(channelindex++), header));
    }
}

int32_t compositionelement::trackLength(const nlohmann::json &j)
{
    std::string track_length_string = j["TrackLength"];
    int32_t track_length = wavfuncs::time_span_to_seconds(track_length_string);
    return track_length;
}

void compositionelement::calculate()
{
    std::ofstream ofstemp;
    try
    {
        char tempfilename_s[FILENAME_MAX] = "wavegencomposition_XXXXXX";
        TMPNAM(tempfilename_s);
        tempfilename = tempfilename_s;
        ofstemp.open(tempfilename, std::ios::trunc | std::ios::binary);
        if (!ofstemp.is_open())
            throw std::runtime_error("Could not open temporary file");

        for (int n = 0; n < header.N; n++)
        {
            double t = (double)header.length_seconds * (double)n / header.N;
            int channelIndex = 0;

            for (std::vector<channel>::iterator channelit = channels.begin(); channelit != channels.end(); channelit++)
            {

                double a = 1;

                std::map<std::string, double> maxGroups;
                std::vector<double> ungroupedComponentValues;

                for (std::vector<FrequencyFunctionWaveFileGroup>::iterator componentit = channelit->components.begin();
                     componentit != channelit->components.end();
                     componentit++)
                {
                    double athis = componentit->Amplitude(t, n);
                    if(isnan(athis) || a < -1 || a > 1)
                    {
                        std::cout << "t=" << t << ", n=" << n << ", athis=" << athis << std::endl;
                        throw std::runtime_error("Component returned NaN or out of range");
                    }
                    a *= athis;
                }

                if (a < -1 || a > 1)
                    throw std::runtime_error("Amplitude must be -1 to 1");

                double newmax = std::max(maxPerChannel[channelIndex], abs(a));
                maxPerChannel[channelIndex] = newmax;
                channelIndex++;

                ofstemp.write(reinterpret_cast<char *>(&a), sizeof(a));
            }
        }

        ofstemp.close();
    }
    catch (const std::exception &e)
    {
        if(ofstemp.is_open()) 
        {
            ofstemp.close();
            remove(tempfilename.c_str());
        }
        throw e;
    }
}
