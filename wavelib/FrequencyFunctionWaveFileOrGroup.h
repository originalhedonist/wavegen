#pragma once
#ifndef CLASS_FREQUENCYFUNCTIONWAVEFILEORGROUP
#define CLASS_FREQUENCYFUNCTIONWAVEFILEORGROUP

class FrequencyFunctionWaveFileOrGroup
{
public:
    virtual double Amplitude(double t, int32_t n) = 0;
    virtual ~FrequencyFunctionWaveFileOrGroup() {};
    virtual bool shouldCalculateForTime(const double& t);
};

#endif
