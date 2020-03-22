#pragma once
class FrequencyFunctionWaveFile
{
public:
    std::string frequency, pulse;
    exprtk::symbol_table<double> symbol_table_frequency;
    exprtk::symbol_table<double> symbol_table_pulse;
    exprtk::expression<double> expression_frequency;
    exprtk::expression<double> expression_pulse;
    exprtk::parser<double> parser;
    double t, n, x;
    double aLast;
    FrequencyFunctionWaveFile(const std::string& frequency, const std::string& pulse);
    double Amplitude(double t, double n);
    double Frequency();
};

