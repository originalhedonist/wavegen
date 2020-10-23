#include "pch.h"
#include "FrequencyFunctionWaveFileGroup.h"

FrequencyFunctionWaveFileGroup::FrequencyFunctionWaveFileGroup(const nlohmann::json j, const std::map<std::string, double>& constants, double channelindex, const headerdata& h)
{
    if(j.contains("SubComponents"))
    {
        j["Aggregation"].get_to(aggregation);
        for(auto sc : j["SubComponents"])
        {
            subcomponents.push_back(FrequencyFunctionWaveFile(sc, constants, channelindex, h));
        }
    }
    else
    {
        aggregation = "max";
        subcomponents.push_back(FrequencyFunctionWaveFile(j, constants, channelindex, h));
    }
}

double FrequencyFunctionWaveFileGroup::Amplitude(double t, int32_t n)
{
    std::vector<double> values;
    for(std::vector<FrequencyFunctionWaveFile>::iterator it = subcomponents.begin(); it != subcomponents.end(); it++)
    {
        values.push_back(it->Amplitude(t, n));
    }

    if(aggregation == "max")
    {
        return *std::max_element(values.begin(), values.end());
    }
    else if(aggregation == "product")
    {
        double a = 1;
        for(auto v : values) a*=v;
        return a;
    }
    else
    {
        std::cerr << "Aggregation = " << aggregation << std::endl;
        throw new std::runtime_error("Unknown aggregation type");
    }   
}