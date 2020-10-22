#include "pch.h"
#include "channel.h"

channel::channel(const nlohmann::json& j, const std::map<std::string, double>& constants, const headerdata& h)
{
    for (auto jc : j["Components"])
    {
        components.push_back(FrequencyFunctionWaveFileGroup(jc, constants, h));
    }
}
