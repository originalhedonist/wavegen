#include "pch.h"
#include "FrequencyFunctionWaveFile.h"

double FrequencyFunctionWaveFile::randomdouble()
{
    return ((double)rand()) / RAND_MAX; 
}

void FrequencyFunctionWaveFile::initialize()
{
    exprtk::parser<double> parser;
    symbol_table_pulse.add_constant("N", h.N);
    symbol_table_pulse.add_variable("t", *t);
    symbol_table_pulse.add_variable("tprev", *tprev);
    symbol_table_pulse.add_variable("n", *n);
    symbol_table_pulse.add_variable("x", *x);
    symbol_table_pulse.add_variable("m", *gradient);
    symbol_table_pulse.add_variable("mprev", *gradientprev);

    symbol_table_pulse.add_function("randomdouble", FrequencyFunctionWaveFile::randomdouble);
    symbol_table_pulse.add_pi();

    symbol_table_frequency.add_constant("N", h.N);
    symbol_table_frequency.add_variable("t", *t);
    symbol_table_frequency.add_variable("n", *n);
    symbol_table_frequency.add_function("randomdouble", FrequencyFunctionWaveFile::randomdouble);
    
    symbol_table_frequency.add_pi();
    expression_pulse.register_symbol_table(symbol_table_pulse);
    expression_frequency.register_symbol_table(symbol_table_frequency);

    for(std::map<std::string, double*>::const_iterator it = variables.begin(); it != variables.end(); it++)
    {
        symbol_table_pulse.add_variable(it->first, *it->second);
        symbol_table_frequency.add_variable(it->first, *it->second); //they're 'shared', between pulse and frequency. probably not a problem...
    }


    if(!parser.compile(frequency, expression_frequency))
    {
        std::cerr << std::endl << frequency << std::endl << std::endl;
        std::cerr << parser.error() << std::endl;
        throw std::runtime_error("Compile error in frequency expression");
    }

    if(!parser.compile(pulse, expression_pulse))
    {
        std::cerr << std::endl << pulse << std::endl << std::endl;
        std::cerr << parser.error() << std::endl;
        throw std::runtime_error("Compile error in pulse expression");
    }
    
    initialized = true;
}

FrequencyFunctionWaveFile::FrequencyFunctionWaveFile(const nlohmann::json j, const headerdata& h) :
    t(new double(0)),
    tprev(new double(0)),
    n(new double(-1)),
    x(new double(0)),
    xprev(new double(0)),
    gradient(new double(0)),
    gradientprev(new double(0)),
    h(h),
    aLast(0),
    initialized(false)
{
    std::string frequencyExpressionOrFile, pulseExpressionOrFile;

    j["Frequency"].get_to(frequencyExpressionOrFile);
    this->frequency = get_expression(frequencyExpressionOrFile);

    j["Pulse"].get_to(pulseExpressionOrFile);
    this->pulse = get_expression(pulseExpressionOrFile);

    std::string varsFile;
    if(j.contains("Variables"))
    {
        j["Variables"].get_to(varsFile);
        if(!varsFile.empty())
        {
            parse_vars(varsFile);
        }
    }

    if(j.contains("MaxGroup"))
    {
        j["MaxGroup"].get_to(maxGroup);
    }
}

void FrequencyFunctionWaveFile::parse_vars(const std::string& varsFile)
{
    std::ifstream file;
    file.open(varsFile);
    if(!file.is_open())
    {
        std::cerr << varsFile << std::endl;
        throw std::runtime_error("Unable to open Variables file");
    }
    std::string line;
    while(getline(file, line))
    {
        double* val = new double(0);
        variables.insert(std::pair<std::string, double*>(line, val));
    }
}

std::string FrequencyFunctionWaveFile::get_expression(const std::string& expression)
{
    const std::string fileEnding = ".exprtk";
    if(expression.length() >= fileEnding.length() && 0 == expression.compare(expression.length() - fileEnding.length(), fileEnding.length(), fileEnding))
    {
        std::ifstream file;
        file.open(expression, std::ios::in);
        if(!file.is_open())
        {
            std::cerr << expression << std::endl;
            throw std::runtime_error("Unable to read file");
        }
        std::ostringstream retval;
        retval << file.rdbuf();
        file.close();

        return retval.str();
    }
    else
    {
        return expression;
    }
}

FrequencyFunctionWaveFile::FrequencyFunctionWaveFile(const FrequencyFunctionWaveFile& other) :
    t(new double(*other.t)),
    tprev(new double(*other.tprev)),
    n(new double(*other.n)),
    x(new double(*other.x)),
    xprev(new double(*other.xprev)),
    gradient(new double(*other.gradient)),
    gradientprev(new double(*other.gradientprev)),
    aLast(other.aLast),
    h(other.h),
    frequency(other.frequency),
    pulse(other.pulse),
    initialized(other.initialized),
    maxGroup(other.maxGroup)
{
    for(auto a : other.variables)
    {
        variables.insert(std::pair<std::string, double*>(a.first, new double(*a.second)));
        // give it its own pointer - as it will be freed. we don't want the copy constructed instance to try and free the same one
    }
}

FrequencyFunctionWaveFile::~FrequencyFunctionWaveFile()
{
    delete n;
    delete t;
    delete tprev;
    delete x;
    delete xprev;
    delete gradient;
    delete gradientprev;
    for(auto a : variables)
    {
        delete a.second;
    }
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

    *tprev = t;
    *xprev = *x;
    *gradientprev = *gradient;
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
