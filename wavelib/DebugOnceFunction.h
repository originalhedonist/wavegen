#pragma once
#ifndef THE_DEBUG_FUNCTION
#define THE_DEBUG_FUNCTION
#include "exprtk.hpp"
#include <iostream>

class debugoncefunction : public exprtk::ivararg_function<double>
{
    bool has_fired = false;
    virtual inline double operator() (const std::vector<double>& params)
    {
        if(!has_fired)
        {
            for(std::vector<double>::const_iterator it = params.begin(); it != params.end(); it++)
            {
                std::cout << (*it) << std::endl;
            }
            has_fired = true;
        }
        return 0;
    }
};

#endif