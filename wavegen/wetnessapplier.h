#pragma once
class wetnessapplier
{
public:
    bool initialized;
    static int nextid;
    int id = nextid++;
    exprtk::symbol_table<double> symbol_table;
    exprtk::expression<double> expression;
    std::string expression_string;
    headerdata h;
    void initialize();
    double* t, * n;
    double aLast;
    wetnessapplier(const std::string& j, const headerdata& h);
    wetnessapplier(const wetnessapplier& other);
    virtual ~wetnessapplier();
    double Amplitude(double t, int32_t n, double baseAmplitude);
};

