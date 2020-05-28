#include "pch.h"
#include "FrequencyFunctionWaveFile.h"

void FrequencyFunctionWaveFile::initialize()
{
    exprtk::parser<double> parser;
    symbol_table_pulse.add_constant("N", h.N);
    symbol_table_pulse.add_variable("t", *t);
    symbol_table_pulse.add_variable("n", *n);
    symbol_table_pulse.add_variable("x", *x);
    symbol_table_pulse.add_variable("m", *gradient);
    symbol_table_pulse.add_variable("mem0", *mem0);
    symbol_table_pulse.add_variable("mem1", *mem1);
    symbol_table_pulse.add_variable("mem2", *mem2);
    symbol_table_pulse.add_variable("mem3", *mem3);
    symbol_table_pulse.add_variable("mem4", *mem4);
    symbol_table_pulse.add_pi();
    symbol_table_frequency.add_constant("N", h.N);
    symbol_table_frequency.add_variable("t", *t);
    symbol_table_frequency.add_variable("n", *n);

    symbol_table_frequency.add_pi();
    expression_pulse.register_symbol_table(symbol_table_pulse);
    expression_frequency.register_symbol_table(symbol_table_frequency);

    parser.compile(frequency, expression_frequency);
    parser.compile(pulse, expression_pulse);
    
    initialized = true;
}

FrequencyFunctionWaveFile::FrequencyFunctionWaveFile(const nlohmann::json j, const headerdata& h) :
    t(new double(0)),
    n(new double(-1)),
    x(new double(0)),
    mem0(new double(0)),
    mem1(new double(0)),
    mem2(new double(0)),
    mem3(new double(0)),
    mem4(new double(0)),
    gradient(new double(0)),
    h(h),
    aLast(0),
    initialized(false)
{
    j["Frequency"].get_to(this->frequency);
    j["Pulse"].get_to(this->pulse);
}

FrequencyFunctionWaveFile::FrequencyFunctionWaveFile(const FrequencyFunctionWaveFile& other) :
    t(new double(*other.t)),
    n(new double(*other.n)),
    x(new double(*other.x)),
    mem0(new double(0)),
    mem1(new double(0)),
    mem2(new double(0)),
    mem3(new double(0)),
    mem4(new double(0)),
    gradient(new double(0)),
    aLast(other.aLast),
    h(other.h),
    frequency(other.frequency),
    pulse(other.pulse),
    initialized(other.initialized)
{
}

FrequencyFunctionWaveFile::~FrequencyFunctionWaveFile()
{
    delete n;
    delete t;
    delete x;
    delete mem0,
    delete mem1,
    delete mem2;
    delete mem3;
    delete mem4;
    delete gradient;
}

double FrequencyFunctionWaveFile::Amplitude(double t, int32_t n)
{
    if (!initialized)
    {
        initialize();
    }

    if (n == *this->n) return aLast;
    if (n != *this->n + 1) throw std::runtime_error("FrequencyFunctionWaveFile::Amplitude called out of sequence");
    *this->n = n;
    *this->t = t;
    double f = Frequency();
    double dx = 2 * M_PI * f / 44100;
    *x += dx;
    double a = this->expression_pulse.value();
    if (isnan(a))
    {
        throw std::runtime_error("Pulse expression returned NaN");
    }
    *gradient = a - aLast;
    aLast = a;
    return a;
}

double FrequencyFunctionWaveFile::Frequency()
{
    if (!initialized)
    {
        initialize();
    }

    double f = expression_frequency.value();
    if (isnan(f))
    {
        throw std::runtime_error("Frequency expression returned NaN");
    }
    return f;
}
