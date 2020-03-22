#pragma once
#include "headerdata.h"

class FrequencyFunctionWaveFile
{
public:
    bool initialized;
    static int nextid;
    int id = nextid++;
    exprtk::symbol_table<double> symbol_table_frequency;
    exprtk::symbol_table<double> symbol_table_pulse;
    exprtk::expression<double> expression_frequency;
    exprtk::expression<double> expression_pulse;
    std::string frequency, pulse;
    headerdata h;
    void initialize();
    double *t, *n, *x;
    double aLast;
    FrequencyFunctionWaveFile(const nlohmann::json j, const headerdata& h);
    FrequencyFunctionWaveFile(const FrequencyFunctionWaveFile& other);
    virtual ~FrequencyFunctionWaveFile();
    double Amplitude(double t, int32_t n);
    double Frequency();
};

