#define _USE_MATH_DEFINES
#define exprtk_disable_caseinsensitivity
#include "stdafx.h"
#include "headerdata.h"
#include "wetnessapplier.h"

int wetnessapplier::nextid = 0;

void wetnessapplier::initialize()
{
    exprtk::parser<double> parser;
    symbol_table.add_constant("N", h.N);
    symbol_table.add_variable("t", *t);
    symbol_table.add_variable("n", *n);

    expression.register_symbol_table(symbol_table);

    parser.compile(expression_string, expression);

    initialized = true;
}

wetnessapplier::wetnessapplier(const std::string& expression_string, const headerdata& h) :
    expression_string(expression_string),
    t(new double(0)),
    n(new double(-1)),
    h(h),
    aLast(0),
    initialized(false)
{
}

wetnessapplier::wetnessapplier(const wetnessapplier& other) :
    t(new double(*other.t)),
    n(new double(*other.n)),
    aLast(other.aLast),
    h(other.h),
    expression_string(other.expression_string),
    initialized(other.initialized)
{
}

wetnessapplier::~wetnessapplier()
{
    delete n;
    delete t;
}

double wetnessapplier::Amplitude(double t, int32_t n, double baseAmplitude)
{
    if (!initialized)
    {
        initialize();
    }

    if (n == *this->n) return aLast;
    if (n != *this->n + 1) throw std::exception("wetnessapplier::Amplitude called out of sequence");
    *this->n = n;
    *this->t = t;

    double apos = 1 - baseAmplitude / 2;
    double wetnessVal = expression.value();
    double dryness = 1 - wetnessVal;
    double a = 1 - dryness * apos;
    aLast = a;
    return a;
}
