#include "pch.h"
#include "channel.h"

channel::channel(const nlohmann::json& j, const std::map<std::string, double>& constants, double channelindex, const headerdata& h, const channelfunction* thechannelfunction)
{
    for (auto jc : j["Components"])
    {
        components.push_back(FrequencyFunctionWaveFileGroup(jc, constants, channelindex, h, thechannelfunction));
    }
}
