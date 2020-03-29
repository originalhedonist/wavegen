#pragma once
#include "headerdata.h"

class wavfuncs
{
public:
    static void write_short(std::ostream& os, int16_t i);
    static void write_int(std::ostream& os, int32_t i);
    static void write_header(std::ofstream& ofs, const headerdata& h);
    static const int32_t time_span_to_seconds(const std::string& timespan);
};

