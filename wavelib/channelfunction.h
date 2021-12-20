#pragma once

#include "pch.h"

class channelfunction : public exprtk::ifunction<double>
{
public:
    double* channelValues;
    long* lastNCheck;
    int numChannels;
    channelfunction(int numChannels);
    channelfunction(const channelfunction& other);
    virtual ~channelfunction();
    inline virtual double operator() (const double& n, const double& channel);
};