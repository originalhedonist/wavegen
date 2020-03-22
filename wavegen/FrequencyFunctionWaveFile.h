#pragma once
#include "headerdata.h"

class FrequencyFunctionWaveFile
{
public:
    std::string frequency, pulse;
    exprtk::symbol_table<double> symbol_table_frequency;
    exprtk::symbol_table<double> symbol_table_pulse;
    exprtk::expression<double> expression_frequency;
    exprtk::expression<double> expression_pulse;
    double t, n, x;
    double aLast;
    FrequencyFunctionWaveFile(const nlohmann::json j, const headerdata& h);
    double Amplitude(double t, double n);
    double Frequency();
};

