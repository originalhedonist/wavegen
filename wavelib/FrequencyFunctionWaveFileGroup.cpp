#include "pch.h"
#include "FrequencyFunctionWaveFileGroup.h"

FrequencyFunctionWaveFileGroup::FrequencyFunctionWaveFileGroup(const nlohmann::json j, const std::map<std::string, double>& constants, double channelindex, const headerdata& h, const channelfunction* thechannelfunction)
{
    // this probably leaks memory - as they are not deleted. can't delete in destructor as copy constructed instance will try again.
    // but process will end soon enough. if it were a web server could use smart pointers
    if(j.contains("SubComponents"))
    {
        j["Aggregation"].get_to(aggregation);
        for(auto sc : j["SubComponents"])
        {
            subcomponents.push_back(new FrequencyFunctionWaveFileGroup(sc, constants, channelindex, h, thechannelfunction));
        }
    }
    else
    {
        aggregation = "max";
        subcomponents.push_back(new FrequencyFunctionWaveFile(j, constants, channelindex, h, thechannelfunction));
    }
}

double FrequencyFunctionWaveFileGroup::Amplitude(double t, int32_t n)
{
    std::vector<double> values;
    for(std::vector<FrequencyFunctionWaveFileOrGroup*>::iterator it = subcomponents.begin(); it != subcomponents.end(); it++)
    {
        FrequencyFunctionWaveFileOrGroup* item = *it;
        if(item->shouldCalculateForTime(t))
        {
            values.push_back(item->Amplitude(t, n));
        }
    }


    if(values.size() == 0) return 0;

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
    else if (aggregation == "sum")
    {
        double a = 0;
        for(auto v : values) a+=v;
        return a;
    }
    else
    {
        std::cerr << "Aggregation = " << aggregation << std::endl;
        throw new std::runtime_error("Unknown aggregation type");
    }   
}