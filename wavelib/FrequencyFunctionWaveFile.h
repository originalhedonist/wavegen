#pragma once
#include "headerdata.h"

class FrequencyFunctionWaveFile
{
public:
    bool initialized;
    
    exprtk::symbol_table<double> symbol_table_frequency;
    exprtk::symbol_table<double> symbol_table_pulse;
    exprtk::expression<double> expression_frequency;
    exprtk::expression<double> expression_pulse;
    std::string frequency, pulse;
    static std::string get_expression(const std::string& expression);

    static double randomdouble();

    headerdata h;
    void initialize();
    double *t, *tprev, *n, *x, *xprev;
    double *gradient, *gradientprev;
    
    std::map<std::string, double*> variables;
    std::map<std::string, double> constants;
    void parse_vars(const std::string& varsFile);

    double aLast;
    FrequencyFunctionWaveFile(const nlohmann::json j, const std::map<std::string, double>& constants, const headerdata& h);
    FrequencyFunctionWaveFile(const FrequencyFunctionWaveFile& other);
    virtual ~FrequencyFunctionWaveFile();
    double Amplitude(double t, int32_t n);
    double Frequency();
};
