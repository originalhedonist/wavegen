#pragma once
#include "channel.h"
#include "headerdata.h"
class compositionelement
{
public:
    std::vector<channel> channels;
    headerdata header;

    compositionelement(const nlohmann::json& json);
    compositionelement(const nlohmann::json& json, const int16_t channels);
    virtual ~compositionelement();

    double get_next(int n, int channel);
    bool is_complete() const;

    void calculate(); //writes out a temp file
    void start();
    void stop();

private:
    int nNext, channelNext; //expected values, used for verification
    double* maxPerChannel;
    char tempfilename[FILENAME_MAX];
    std::ifstream ifstemp;

    static int32_t trackLength(const nlohmann::json& j);
};

