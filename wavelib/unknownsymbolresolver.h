#include "exprtk.hpp"
#include <vector>
#include <string>

class unknownsymbolresolver : public exprtk::parser<double>::unknown_symbol_resolver
{
    public:
    std::vector<std::string> unknownsymbols;

    virtual bool process(const std::string& unknown_symbol,
                    exprtk::parser<double>::unknown_symbol_resolver::usr_symbol_type&   st,
                    double&                 default_value,
                    std::string&       error_message)
    {
        unknownsymbols.push_back(unknown_symbol);
        return false;
    }
};
