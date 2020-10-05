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
}

int32_t compositionelement::trackLength(const nlohmann::json& j)
{
    std::string track_length_string = j["TrackLength"];
    int32_t track_length = wavfuncs::time_span_to_seconds(track_length_string);
    return track_length;
}

void compositionelement::calculate()
{
    char tempfilename_s[FILENAME_MAX] = "wavegencomposition_XXXXXX";
    TMPNAM(tempfilename_s);
    tempfilename = tempfilename_s;
    std::ofstream ofstemp;
    ofstemp.open(tempfilename, std::ios::trunc | std::ios::binary);
    if (!ofstemp.is_open()) throw std::runtime_error("Could not open temporary file");

    for (int n = 0; n < header.N; n++)
    {
        double t = (double)header.length_seconds * (double)n / header.N;
        int channelIndex = 0;

        for (std::vector<channel>::iterator channelit = channels.begin(); channelit != channels.end(); channelit++)
        {

            double a = 1;

            std::map<std::string, double> maxGroups;
            std::vector<double> ungroupedComponentValues;

            for (std::vector<FrequencyFunctionWaveFile>::iterator componentit = channelit->components.begin();
                componentit != channelit->components.end();
                componentit++)
            {
                auto athis = componentit->Amplitude(t, n);

                if(!componentit->maxGroup.empty())
                {
                    if(maxGroups.find(componentit->maxGroup) == maxGroups.end())
                    {
                        maxGroups.insert(std::pair<std::string, int>(componentit->maxGroup, 0));
                    }
                    maxGroups[componentit->maxGroup] = std::max(maxGroups[componentit->maxGroup], athis);
                }
                else
                {
                    ungroupedComponentValues.push_back(athis); //this is to apply the ungrouped components (typically the carrier) AFTER the max'ed groups
                    // reason being, because the max'ed groups will typically be positive only (0 - 1), but the carrier applies the negativity (-1 to 1)
                }
            }

            for(auto m : maxGroups)
            {
                a *= m.second;
            }

            for(auto v : ungroupedComponentValues)
            {
                a *= v;
            }

            if (a < -1 || a > 1) throw std::runtime_error("Amplitude must be -1 to 1");

            double newmax = std::max(maxPerChannel[channelIndex], abs(a));
            maxPerChannel[channelIndex] = newmax;
            channelIndex++;

            ofstemp.write(reinterpret_cast<char*>(&a), sizeof(a));
        }
    }

    ofstemp.close();
}
