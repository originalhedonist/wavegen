#pragma once
#include "headerdata.h"
#include "compositionreader.h"

class wavfuncs
{
public:
    static void write_short(std::ostream& os, int16_t i);
    static void write_int(std::ostream& os, int32_t i);
    static void write_header(std::ofstream& ofs, const headerdata& h);
    static const int32_t time_span_to_seconds(std::string timespan);
    static const std::string seconds_to_timespan(int32_t seconds);
    static const nlohmann::json read_json(const std::string& file);
    static void write_wav(std::ostream& ofs, const headerdata& hOverall, int channels, compositionreader& ce);
    static void write_double_as_short(std::ostream& ofs, double a);
};

