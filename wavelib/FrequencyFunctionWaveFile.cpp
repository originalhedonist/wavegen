#include "pch.h"
#include "FrequencyFunctionWaveFile.h"
#include "unknownsymbolresolver.h"

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


FrequencyFunctionWaveFile::FrequencyFunctionWaveFile(const nlohmann::json j, const std::map<std::string, double>& constants, double channelindex, const headerdata& h, channelfunction* thechannelfunction, bool calculationOnly) :
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
    everFiltered(false),
    thechannelfunction(thechannelfunction),
    calculationOnly(calculationOnly)
{
    std::string frequencyExpressionOrFile, pulseExpressionOrFile;

    j["Frequency"].get_to(frequencyExpressionOrFile);
    this->frequency = get_expression(frequencyExpressionOrFile);

    if (j.contains("FrequencySubstitutions"))
    {
        std::string frequencySubstitutions;
        j["FrequencySubstitutions"].get_to(frequencySubstitutions);
        make_substitutions(this->frequency, frequencySubstitutions);
    }

    j["Pulse"].get_to(pulseExpressionOrFile);
    this->pulse = get_expression(pulseExpressionOrFile);

    if (j.contains("PulseSubstitutions"))
    {
        std::string pulseSubstitutions;
        j["PulseSubstitutions"].get_to(pulseSubstitutions);
        make_substitutions(this->pulse, pulseSubstitutions);
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

void FrequencyFunctionWaveFile::make_substitutions(std::string& input, const std::string& substitutions)
{
    std::string retval(input);
    std::istringstream subsstream(substitutions);
    std::string substitution;
    while (std::getline(subsstream, substitution, ';'))
    {
        size_t equalspos = substitution.find('=');
        if (equalspos == std::string::npos)
        {
            std::cerr << substitutions << std::endl;
            std::cerr << substitution << std::endl;
            throw std::runtime_error("Invalid substitution");
        }
        const std::string from = substitution.substr(0, equalspos);
        const std::string to = substitution.substr(equalspos + 1);
        replace_all(input, from, to);
    }
}

void FrequencyFunctionWaveFile::replace_all(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
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
    f(other.f),
    h(other.h),
    frequency(other.frequency),
    pulse(other.pulse),
    initialized(other.initialized),
    _constants(other._constants),
    channelindex(other.channelindex),
    startTime(other.startTime),
    endTime(other.endTime),
    everFiltered(other.everFiltered),
    thechannelfunction(other.thechannelfunction),
    calculationOnly(other.calculationOnly)
{
}

FrequencyFunctionWaveFile::~FrequencyFunctionWaveFile()
{
}

template<typename T>
struct boo : public exprtk::ivararg_function<T>
{
    inline T operator()(const std::vector<T>& arglist)
    {
        T result = T(0);
        std::cout << "In vararg function!" << std::endl;
        for (std::size_t i = 0; i < arglist.size(); ++i)
        {
            result += arglist[i] / arglist[i > 0 ? (i - 1) : 0];
        }
        return 0.7;
        //return result;
    }
};

void FrequencyFunctionWaveFile::initialize()
{
    exprtk::parser<double> parser_frequency, parser_pulse;
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
    symbol_table_pulse.add_function("channel", *thechannelfunction);
    symbol_table_pulse.add_function("mixin", themixinfunction);
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
    symbol_table_frequency.add_function("channel", *thechannelfunction);
    

    /// <summary>
    /// 
    /// </summary>
    symbol_table_frequency.add_constant("channelindex", channelindex);
    symbol_table_pulse.add_constant("channelindex", channelindex);

    //themixinfunction.somefunc();

    for(std::map<std::string, double>::const_iterator it = _constants.begin(); it != _constants.end(); it++)
    {
        symbol_table_frequency.add_constant(it->first, it->second);
        symbol_table_pulse.add_constant(it->first, it->second);
    }

    symbol_table_frequency.add_pi();
    expression_pulse.register_symbol_table(symbol_table_pulse);
    expression_frequency.register_symbol_table(symbol_table_frequency);

    compile("frequency", frequency, symbol_table_frequency, expression_frequency);
    compile("pulse", pulse, symbol_table_pulse, expression_pulse);
    
    initialized = true;
}

void FrequencyFunctionWaveFile::compile(const std::string& description, const std::string& expression_string, exprtk::symbol_table<double> symbol_table   /* invokes copy constructor */, exprtk::expression<double>& expression)
{
    for(std::vector<std::string> missing_variables = FrequencyFunctionWaveFile::get_missing_variables(symbol_table, expression_string);
        missing_variables.size() > 0;
        missing_variables = FrequencyFunctionWaveFile::get_missing_variables(symbol_table, expression_string))
    {
        for(auto mv : missing_variables)
        {
            std::map<std::string, double*>::const_iterator var = variables.find(mv);
            double* val;
            if(var != variables.end())
            {
                val = var->second;
            }
            else
            {
                val = new double(0);
                std::cout << id << ": adding implicit variable " << mv << std::endl;
                variables.insert(std::pair<std::string, double*>(mv, val));
            }
            symbol_table.add_variable(mv, *val);
        }
    }

    exprtk::parser<double> parser;
    if(!parser.compile(expression_string, expression))
    {
        std::cerr << parser.error() << std::endl;
        std::cerr << "Compile error in " << description << " expression:" << std::endl;
        std::cerr << expression_string << std::endl;
        throw std::runtime_error("Compile failed");
    }
}

std::vector<std::string> FrequencyFunctionWaveFile::get_missing_variables(exprtk::symbol_table<double> symbol_table  /* invokes copy constructor */, const std::string& expression_string)
{
    exprtk::parser<double> parser;
    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);
    unknownsymbolresolver usr;
    parser.enable_unknown_symbol_resolver(usr);
    parser.compile(expression_string, expression);
    return usr.unknownsymbols;
}

int FrequencyFunctionWaveFile::nextid = 0;

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
    if(!calculationOnly && (a < -1 || a > 1))
    {
        std::cerr << "n=" << n << ", a=" << a << std::endl;
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
