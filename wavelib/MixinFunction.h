#pragma once

#ifndef CLASS_MIXINFUNCTION
#define CLASS_MIXINFUNCTION

#include "exprtk.hpp"

class mixinfunction : public exprtk::ivararg_function<double>
{
public:
    virtual inline double operator() (const std::vector<double>& params)
    {
        double t = params[0];
        double T = params[1];
        double value = 0;
        double max = 0;
        if (t == 0) return 0;

        for (size_t i = 3; i < params.size(); i += 2)
        {
            double component = params[i- 1];
            double start = params[i];
            if (t >= start)
            {
                double proportion = (t - start) / (T - start);
                value += proportion * component;
                max += proportion;
            }
        }
        double retval = value / max;
        return retval;
    }

    virtual ~mixinfunction() {}
};

#endif
