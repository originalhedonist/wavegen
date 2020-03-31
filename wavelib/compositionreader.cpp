#include "pch.h"
#include "compositionreader.h"
#include "wavfuncs.h"
using json = nlohmann::json;

int compositionreader::nextid = 0;

compositionreader::compositionreader(const compositionelement& element) :
    maxPerChannel(element.maxPerChannel),
    header(element.header),
    channelNext(0),
    nNext(0)
{
    ifstemp.open(element.tempfilename, std::ios::binary);
    if (!ifstemp.is_open()) throw std::exception("Could not read temp file");
}

compositionreader::~compositionreader()
{
    ifstemp.close();
}

double compositionreader::get_next(int n, int16_t channel)
{
    if (n != nNext)
    {
        throw std::exception("Called get_next with n out of sequence");
    }
    if (channel != channelNext)
    {
        throw std::exception("Called get_next with channel out of sequence");
    }

    double a;
    ifstemp.read(reinterpret_cast<char*>(&a), sizeof(a));
    double aNorm = a / maxPerChannel[channel]; //normalization
    if (++channelNext >= header.channels)
    {
        nNext++;
        channelNext = 0;
    }
    return aNorm;
}

bool compositionreader::is_complete() const
{
    return nNext >= header.N;
}
