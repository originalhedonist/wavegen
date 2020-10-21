#include "pch.h"
#include "channel.h"

channel::channel(const nlohmann::json& j, const headerdata& h)
{
    for (auto jc : j["Components"])
    {
        components.push_back(FrequencyFunctionWaveFileGroup(jc, h));
    }
}
