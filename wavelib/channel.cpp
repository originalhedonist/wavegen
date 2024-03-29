#include "pch.h"
#include "channel.h"

channel::channel(const nlohmann::json& j, const std::map<std::string, double>& constants, double channelindex, const headerdata& h, channelfunction* thechannelfunction)
    : calculationOnly(false), normalize(true)
{
    if(j.contains("CalculationOnly")) j["CalculationOnly"].get_to(calculationOnly);
    if(j.contains("Normalize")) j["Normalize"].get_to(normalize);
    
    for (auto jc : j["Components"])
    {
        components.push_back(FrequencyFunctionWaveFileGroup(jc, constants, channelindex, h, thechannelfunction, calculationOnly));
    }
}
