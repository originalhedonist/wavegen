#include "pch.h"
#include "FrequencyFunctionWaveFile.h"

std::map<std::string, double*> FrequencyFunctionWaveFile::variables;

double FrequencyFunctionWaveFile::randombetween(double bottom, double top)
{
    return bottom + (FrequencyFunctionWaveFile::randomdouble() * (top - bottom));
}

double FrequencyFunctionWaveFile::randomdouble()
{
    return ((double)rand()) / RAND_MAX; 
}

double FrequencyFunctionWaveFile::sinorcos(double channelindex, double arg)
{
    return channelindex == 1 ? sin(arg) : cos(arg);
}

FrequencyFunctionWaveFile::FrequencyFunctionWaveFile(const nlohmann::json j, const std::map<std::string, double>& constants, double channelindex, const headerdata& h) :
    t(new double(0)),
    tprev(new double(0)),
    n(new double(-1)),
    x(new double(0)),
    xprev(new double(0)),
    f(new double(0)),
    gradient(new double(0)),
    gradientprev(new double(0)),
    h(h),
    aLast(0),
    initialized(false),
    _constants(constants),
    channelindex(channelindex),
    startTime(-1),
    endTime(-1),
    everFiltered(false)
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

    for(nlohmann::json::const_iterator it = j.begin(); it != j.end(); it++)
    {
        if(strcmp(it.key().substr(0,1).c_str(), ":") == 0)
        {
            auto constname = it.key().substr(1);
            double constval = 0;
            it.value().get_to(constval);
            _constants.insert(std::pair<std::string, double>(constname, constval));

            if(it.key().compare(":starttime") == 0)
            {
                startTime = constval;
            }
            if(it.key().compare(":endtime") == 0)
            {
                endTime = constval;
            }
        }
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
    _constants(other._constants),
    channelindex(other.channelindex),
    startTime(other.startTime),
    endTime(other.endTime),
    everFiltered(other.everFiltered)
{
}

FrequencyFunctionWaveFile::~FrequencyFunctionWaveFile()
{
}

class unknownsymbolresolver : public exprtk::parser<double>::unknown_symbol_resolver
{
public:
    std::vector<std::string> unknownsymbols;

    bool process(const std::string& unknown_symbol,
                            usr_symbol_type&   st,
                            double&            default_value,
                            std::string&       error_message)
    {
        std::cout << "unknown symbol caught: " << unknown_symbol << std::endl;
        unknownsymbols.push_back(unknown_symbol);
        return false;
    }

        // virtual bool process(const std::string& unknown_symbol,
        //                     exprtk::symbol_table<double>& symbol_table,
        //                     std::string& error_message)
        // {
        //     std::cout << "process2 " << unknown_symbol << " error = " << error_message << std::endl;
        // return false;
        // }

};

void FrequencyFunctionWaveFile::initialize()
{
    
    symbol_table_pulse.add_constant("N", h.N);
    symbol_table_pulse.add_constant("T", h.length_seconds);
    symbol_table_pulse.add_variable("t", *t);
    symbol_table_pulse.add_variable("tprev", *tprev);
    symbol_table_pulse.add_variable("n", *n);
    symbol_table_pulse.add_variable("x", *x);
    symbol_table_pulse.add_variable("m", *gradient);
    symbol_table_pulse.add_variable("mprev", *gradientprev);
    symbol_table_pulse.add_variable("f", *f);
    symbol_table_pulse.add_function("randomdouble", FrequencyFunctionWaveFile::randomdouble);
    symbol_table_pulse.add_function("randombetween", FrequencyFunctionWaveFile::randombetween);
    symbol_table_pulse.add_function("sinorcos", FrequencyFunctionWaveFile::sinorcos);
    symbol_table_pulse.add_pi();

    symbol_table_frequency.add_constant("N", h.N);
    symbol_table_frequency.add_constant("T", h.length_seconds);
    symbol_table_frequency.add_variable("t", *t);
    symbol_table_frequency.add_variable("n", *n);
    symbol_table_frequency.add_variable("m", *gradient);
    symbol_table_frequency.add_variable("mprev", *gradientprev);
    symbol_table_frequency.add_function("randomdouble", FrequencyFunctionWaveFile::randomdouble);
    symbol_table_frequency.add_function("randombetween", FrequencyFunctionWaveFile::randombetween);
    symbol_table_frequency.add_function("sinorcos", FrequencyFunctionWaveFile::sinorcos);

    symbol_table_frequency.add_constant("channelindex", channelindex);
    symbol_table_pulse.add_constant("channelindex", channelindex);

    for(std::map<std::string, double>::const_iterator it = _constants.begin(); it != _constants.end(); it++)
    {
        symbol_table_frequency.add_constant(it->first, it->second);
        symbol_table_pulse.add_constant(it->first, it->second);
    }

    symbol_table_frequency.add_pi();

    for(std::map<std::string, double*>::const_iterator it = variables.begin(); it != variables.end(); it++)
    {
        symbol_table_pulse.add_variable(it->first, *it->second);
        symbol_table_frequency.add_variable(it->first, *it->second); //they're 'shared', between pulse and frequency. probably not a problem...
    }

    if(!trycompile(frequency, symbol_table_frequency, expression_frequency))
    {
        std::cerr << std::endl << frequency << std::endl << std::endl;
        throw std::runtime_error("Compile error in frequency expression");
    }
    
    if(!trycompile(pulse, symbol_table_pulse, expression_pulse))
    {
        std::cerr << std::endl << pulse << std::endl << std::endl;
        throw std::runtime_error("Compile error in pulse expression");
    }

    expression_pulse.register_symbol_table(symbol_table_pulse);
    expression_frequency.register_symbol_table(symbol_table_frequency);

    initialized = true;
}

bool FrequencyFunctionWaveFile::trycompile(const std::string& expression_string, exprtk::symbol_table<double>& symbol_table, exprtk::expression<double>& expression)
{
    exprtk::parser<double> parser;
    unknownsymbolresolver usr;
    parser.enable_unknown_symbol_resolver(usr);
    if(!parser.compile(expression_string, expression))
    {
        std::cerr << parser.error() << std::endl;
        return false;
    }

    while(usr.unknownsymbols.size() > 0)
    {
        for(auto s : usr.unknownsymbols)
        {
            std::cout << "Adding unknown symbol " << s << std::endl;
            double* val = new double(0);
            variables.insert(std::pair<std::string, double*>(s, val));

            symbol_table.add_variable(s, *val);
        }
    }

    return true;
}

int FrequencyFunctionWaveFile::nextid = 0;

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

bool FrequencyFunctionWaveFile::shouldCalculateForTime(const double& t)
{
    if(endTime > startTime && startTime >= 0)
    {
        // are they defined? (default to -1)
        // if so, filter.
        bool include = t >= startTime && t <= endTime;
        if(!include) everFiltered = true;

        return include;
    }

    return true;
}

double FrequencyFunctionWaveFile::Amplitude(double t, int32_t n)
{
    if (!initialized)
    {
        initialize();
    }

    if (n == *this->n) return aLast;

    if(!everFiltered)
    {
        if (n != *this->n + 1) 
        {
            std::cout << "FrequencyFunctionWaveFile::Amplitude called out of sequence" << std::endl;
            throw std::runtime_error("FrequencyFunctionWaveFile::Amplitude called out of sequence");
        }
    }
    *this->n = n;
    *this->t = t;
    *f = Frequency();
    double dx = 2 * M_PI * (*f) / 44100;
    *x += dx;
    double a = this->expression_pulse.value();
    if (std::isnan(a))
    {
        throw std::runtime_error("Pulse expression returned NaN");
    }
    if(a < -1 || a > 1)
    {
        std::cout << "a=" << a << std::endl;
        throw std::runtime_error("Pulse expression returned out of range value");
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
    if (std::isnan(f))
    {
        throw std::runtime_error("Frequency expression returned NaN");
    }
    return f;
}
