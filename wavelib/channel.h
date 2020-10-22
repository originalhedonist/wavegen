#pragma once
#include "headerdata.h"
#include "FrequencyFunctionWaveFileGroup.h"

class channel
{
public:
    std::vector<FrequencyFunctionWaveFileGroup> components;
    channel(const nlohmann::json& j, const std::map<std::string, double>& constants, const headerdata& h);
};

