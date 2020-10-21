#include "pch.h"
#include "FrequencyFunctionWaveFileGroup.h"

FrequencyFunctionWaveFileGroup::FrequencyFunctionWaveFileGroup(const nlohmann::json j, const headerdata& h)
{
    if(j.contains("SubComponents"))
    {
        j["Aggregation"].get_to(aggregation);
        for(auto sc : j["SubComponents"])
        {
            subcomponents.push_back(FrequencyFunctionWaveFile(sc, h));
        }
    }
    else
    {
        aggregation = "max";
        subcomponents.push_back(FrequencyFunctionWaveFile(j, h));
    }
}