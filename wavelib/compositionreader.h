#pragma once
#include "channel.h"
#include "headerdata.h"
#include "compositionelement.h"

class compositionreader
{
public:
    compositionreader(const compositionelement& element);
    virtual ~compositionreader();

    double get_next(int n, int16_t channel);
    bool is_complete() const;
    headerdata header;

private:
    std::map<int16_t, double> maxPerChannel;
    int nNext, channelNext; //expected values, used for verification
    std::ifstream ifstemp;
};
