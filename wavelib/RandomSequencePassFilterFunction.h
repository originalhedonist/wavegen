#pragma once
#ifndef RANDOMSEQUENCEPASSFILTER_FUNCTION
#define RANDOMSEQUENCEPASSFILTER_FUNCTION
#include "exprtk.hpp"

class randomsequencepassfilterfunction : public exprtk::ivararg_function<double>
{
    double lastval = 0;
    double thisval = 0; // whether filtered or not
    int sequence = 0;
    int nextsequence = -1;

    virtual inline double operator() (const std::vector<double>& params)
    {
        thisval = params[0];
        long skipmin = (long)params[1];
        long skipmax = (long)params[2];
        
        if((thisval >= 0 && lastval < 0) || (thisval <= 0 && lastval > 0))
        {
            sequence++;
            if(sequence > nextsequence)
            {
                 double r = ((double)rand()) / RAND_MAX;
                 long skip = skipmin + (long)(r * (double)(skipmax - skipmin));
                 nextsequence = sequence + skip;
            }
        }
        
        bool pass = sequence == nextsequence;
        lastval = thisval;
        return pass ? thisval : 0;
    }
};

#endif