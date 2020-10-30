#include "headerdata.h"
#include "FrequencyFunctionWaveFile.h"

class FrequencyFunctionWaveFileGroup : public FrequencyFunctionWaveFileOrGroup
{
public:
    std::string aggregation;
    std::vector<FrequencyFunctionWaveFileOrGroup*> subcomponents;
    FrequencyFunctionWaveFileGroup(const nlohmann::json j, const std::map<std::string, double>& constants, double channelindex, const headerdata& h);
    double Amplitude(double t, int32_t n);
};
