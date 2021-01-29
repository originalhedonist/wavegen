#pragma once
#include "channel.h"
#include "headerdata.h"
#include "channelfunction.h"

class compositionelement
{
public:
    std::vector<channel> channels;
    headerdata header;

    channelfunction thechannelfunction;
    int16_t totalChannels, writeableChannels;
    compositionelement(const nlohmann::json& json, const std::map<std::string, double>& constants);
    compositionelement(const nlohmann::json& json, const std::map<std::string, double>& constants, int16_t totalChannels, int16_t writeableChannels);

    void calculate(); //writes out a temp file

    std::map<std::string, double> constants;
    std::map<int16_t, double> maxPerChannel;
    std::string tempfilename;

private:
    int nNext, channelNext; //expected values, used for verification
    std::ifstream ifstemp;

    static int32_t trackLength(const nlohmann::json& j);
    static int16_t countWritableChannels(const nlohmann::json& j); // as opposed to ones that are just calculation only
};

