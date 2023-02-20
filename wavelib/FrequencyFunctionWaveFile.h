#pragma once
#include "headerdata.h"
#include "exprtk.hpp"
#include "channelfunction.h"

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

class FrequencyFunctionWaveFileOrGroup
{
public:
    virtual double Amplitude(double t, int32_t n) = 0;
    virtual ~FrequencyFunctionWaveFileOrGroup() {};
    virtual bool shouldCalculateForTime(const double& t)
    {
        return true;
    }
};

class FrequencyFunctionWaveFile : public FrequencyFunctionWaveFileOrGroup
{
public:
    static int nextid;
    int id = nextid++;

    channelfunction* thechannelfunction;
    mixinfunction themixinfunction;
    normalizefunction thenormalizefunction;

    FrequencyFunctionWaveFile(const nlohmann::json j, const std::map<std::string, double>& constants, double channel, const headerdata& h, channelfunction* thechannelfunction, bool calculationOnly);
    FrequencyFunctionWaveFile(const FrequencyFunctionWaveFile& other);

    bool initialized;
    bool calculationOnly;
    double startTime; // don't process it before this time. optimization for Breaks2 file (json autogenerated from numbers)
    double endTime; // don't process it after this time. optimization for Breaks2
    bool everFiltered;
    exprtk::symbol_table<double> symbol_table_frequency;
    exprtk::symbol_table<double> symbol_table_pulse;
    exprtk::expression<double> expression_frequency;
    exprtk::expression<double> expression_pulse;

    void compile(const std::string& description, const std::string& expression_string, exprtk::symbol_table<double> symbol_table   /* invokes copy constructor */, exprtk::expression<double>& expression);

    std::string frequency, pulse;
    static std::string get_expression(const std::string& expression);
    static std::vector<std::string> get_missing_variables(exprtk::symbol_table<double> symbol_table, const std::string& expression);
    static double randomdouble();
    static double randomdoublebetween(double bottom, double top);
    static double sinorcos(double index, double arg);

    static void make_substitutions(std::string& input, const std::string& substitutions);
    static void replace_all(std::string& str, const std::string& from, const std::string& to);

    headerdata h;
    void initialize();
    double *t, *tprev, *n, *x, *xprev, *f;
    double *gradient, *gradientprev;
    double channelindex;

    std::map<std::string, double*> variables;
    std::map<std::string, double> _constants;

    double aLast;
    virtual ~FrequencyFunctionWaveFile();
    double Amplitude(double t, int32_t n);
    double Frequency();
    bool shouldCalculateForTime(const double& t);
};
