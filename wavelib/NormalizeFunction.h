#pragma once
#ifndef CLASS_NORMALIZEFUNCTION
#define CLASS_NORMALIZEFUNCTION
#include "exprtk.hpp"
#include <queue>

class normalizationlist
{
public:
    virtual ~normalizationlist() {}
    std::multiset<double> values;
    std::queue<std::multiset<double>::const_iterator> generations;

    void add(double value, int toKeep)
    {
        if(toKeep < 2)
        {
            throw std::runtime_error("toKeep must be at least 2.");
        }

        generations.push(values.insert(value));
        if(generations.size() > toKeep)
        {
            values.erase(generations.front());
            generations.pop();
        }
    }

    double getmax() const {return *values.rbegin();}
    double getmin() const {return *values.begin();}
};


class normalizefunction : public exprtk::ivararg_function<double>
{
    normalizationlist thelist;
public:
    
    virtual inline double operator() (const std::vector<double>& params)
    {
        try
        {
            int n = (long)params[0];
            int scan = (long)params[1];
            double val = params[2];

            thelist.add(val, scan);

            double maxVal = thelist.getmax();
            double minVal = thelist.getmin();

            double retval;
            if(maxVal - minVal != 0)
            {
                double proportion = (val - minVal) / (maxVal - minVal);
                retval = (proportion * 2) - 1;
            }
            else retval = val;

            return retval;
        }
        catch(const std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            return 0;
        }
    }

    virtual ~normalizefunction() {}
};

#endif
