#pragma once
class channel
{
public:
    wetnessapplier wetness;
    FrequencyFunctionWaveFile carrier;
    std::vector<FrequencyFunctionWaveFile> components;
    channel(const nlohmann::json& j, const headerdata& h);
};

