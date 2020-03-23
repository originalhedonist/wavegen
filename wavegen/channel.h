#pragma once
#include "headerdata.h"
#include "FrequencyFunctionWaveFile.h"

class channel
{
public:
    std::vector<FrequencyFunctionWaveFile> components;
    channel(const nlohmann::json& j, const headerdata& h);
};

