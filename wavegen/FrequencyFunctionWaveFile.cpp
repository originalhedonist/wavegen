#define _USE_MATH_DEFINES
#define exprtk_disable_caseinsensitivity
#include "stdafx.h"
#include "FrequencyFunctionWaveFile.h"

FrequencyFunctionWaveFile::FrequencyFunctionWaveFile(const nlohmann::json j, const headerdata& h)
: t(0), n(-1), x(0), aLast(0)
{
    exprtk::parser<double> parser;
    std::string frequency = j["Frequency"];
    std::string pulse = j["Pulse"];
    symbol_table_pulse.add_constant("N", h.N);
    symbol_table_pulse.add_variable("t", t);
    symbol_table_pulse.add_variable("n", n);
    symbol_table_pulse.add_variable("x", x);

    symbol_table_frequency.add_constant("N", h.N);
    symbol_table_frequency.add_variable("t", t);
    symbol_table_frequency.add_variable("n", n);

    expression_pulse.register_symbol_table(symbol_table_pulse);
    expression_frequency.register_symbol_table(symbol_table_frequency);

    parser.compile(frequency, expression_frequency);
    parser.compile(pulse, expression_pulse);
}

double FrequencyFunctionWaveFile::Amplitude(double t, double n)
{
    if (n == this->n) return aLast;
    if (n != this->n + 1) throw std::exception("FrequencyFunctionWaveFile::Amplitude called out of sequence");
    this->n = n;
    this->t = t;
    double f = Frequency();
    double dx = 2 * M_PI * f / 44100;
    x += dx;
    double a = this->expression_pulse.value();
    aLast = a;
    return a;
}

double FrequencyFunctionWaveFile::Frequency()
{
    return expression_frequency.value();
}
