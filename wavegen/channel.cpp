#include "stdafx.h"
#include "headerdata.h"
#include "FrequencyFunctionWaveFile.h"
#include "channel.h"

channel::channel(const nlohmann::json& j, const headerdata& h) : 
    carrier((nlohmann::json)j["Carrier"], h)
{
    j["Wetness"].get_to(wetness);
    for (auto jc : j["Components"])
    {
        components.push_back(FrequencyFunctionWaveFile(jc, h));
    }
}
