#include "stdafx.h"
#include "headerdata.h"
#include "wetnessapplier.h"
#include "FrequencyFunctionWaveFile.h"
#include "channel.h"

channel::channel(const nlohmann::json& j, const headerdata& h) : 
    carrier(j["Carrier"], h),
    wetness(j["Wetness"], h)
{
    for (auto jc : j["Components"])
    {
        components.push_back(FrequencyFunctionWaveFile(jc, h));
    }
}
