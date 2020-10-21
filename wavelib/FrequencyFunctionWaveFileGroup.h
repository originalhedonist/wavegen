#include "headerdata.h"
#include "FrequencyFunctionWaveFile.h"

class FrequencyFunctionWaveFileGroup
{
public:
    std::string aggregation;
    std::vector<FrequencyFunctionWaveFile> subcomponents;
    FrequencyFunctionWaveFileGroup(const nlohmann::json j, const headerdata& h);
};
