#include "headerdata.h"
#include "FrequencyFunctionWaveFile.h"

class FrequencyFunctionWaveFileGroup
{
public:
    std::string aggregation;
    std::vector<FrequencyFunctionWaveFile> subcomponents;
    FrequencyFunctionWaveFileGroup(const nlohmann::json j, const std::map<std::string, double>& constants, const headerdata& h);
    double Amplitude(double t, int32_t n);
};
