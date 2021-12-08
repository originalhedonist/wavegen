#include "pch.h"
#include "channelfunction.h"

channelfunction::channelfunction(const channelfunction& other) : channelfunction(other.numChannels) {}

channelfunction::channelfunction(int numChannels) : exprtk::ifunction<double>(2), numChannels(numChannels) // 2 parameters (n, channel)
{ 
    channelValues = new double[numChannels];
    lastNCheck = new long[numChannels];
    memset(lastNCheck, 0, sizeof(long)*numChannels);
    memset(channelValues, 0, sizeof(double)*numChannels);
}

channelfunction::~channelfunction()
{
    delete[] channelValues;
    delete[] lastNCheck;
}

double channelfunction::operator() (const double& n, const double& channel)
{
    if(n != lastNCheck[(int)channel])
    {
        std::cerr << "called channelfunction(" << n << ", " << channel << ") but last value written for this channel was for " << lastNCheck[(int)channel] << std::endl;
        throw std::runtime_error("channelfunction inconsistency");
    }

    return channelValues[(int)channel];
}
