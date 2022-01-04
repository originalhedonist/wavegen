#pragma once
#ifndef CLASS_NORMALIZEFUNCTION
#define CLASS_NORMALIZEFUNCTION
#include "exprtk.hpp"

class normalizefunction : public exprtk::ivararg_function<double>
{
public:
    static const int NORMALIZE_MEM_SIZE = 44100*10; //10 seconds

    double mem[NORMALIZE_MEM_SIZE]; // a second
    normalizefunction()
    {
        memset(mem, 0, sizeof(double) * NORMALIZE_MEM_SIZE);
    }

    virtual inline double operator() (const std::vector<double>& params)
    {
        int n = (long)params[0];
        int scan = (long)params[1];
        if(scan <= 0 || scan > NORMALIZE_MEM_SIZE)
        {
            throw std::runtime_error("Scan size must be between 1 and NORMALIZE_MEM_SIZE.");
        }
        double val = params[2];
        int nBase = n % scan;
        mem[nBase] = val;
        double maxVal = *std::max_element(mem, mem + scan);        
        double minVal = *std::min_element(mem, mem + scan);
        if(maxVal > minVal && val >= minVal && maxVal >= val)
        {
            double proportion = (val - minVal) / (maxVal - minVal);
            return (proportion * 2) - 1;
        }
        else return val;
    }

    virtual ~normalizefunction() {}
};

#endif
