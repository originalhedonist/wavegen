#pragma once
#include "channel.h"
#include "headerdata.h"
class compositionelement
{
public:
    std::vector<channel> channels;
    headerdata header;

    compositionelement(const nlohmann::json& json, const std::map<std::string, double>& constants);
    compositionelement(const nlohmann::json& json, const std::map<std::string, double>& constants, const int16_t channels);

    void calculate(); //writes out a temp file

    std::map<std::string, double> constants;
    std::map<int16_t, double> maxPerChannel;
    std::string tempfilename;

private:
    int nNext, channelNext; //expected values, used for verification
    std::ifstream ifstemp;

    static int32_t trackLength(const nlohmann::json& j);
};

