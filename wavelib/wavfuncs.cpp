#include "pch.h"
#include "wavfuncs.h"

void wavfuncs::write_short(std::ostream& os, int16_t i)
{
    os << (BYTE)(i & 0xff);
    os << (BYTE)((i >> 8) & 0xff);
}

void wavfuncs::write_int(std::ostream& os, int32_t i)
{
    os << (BYTE)(i & 0xff);
    os << (BYTE)((i >> 8) & 0xff);
    os << (BYTE)((i >> 16) & 0xff);
    os << (BYTE)((i >> 24) & 0xff);
}

void wavfuncs::write_header(std::ofstream& ofs, const headerdata& h)
{
    ofs.write("RIFF", 4);
    write_int(ofs, h.overallFileSize - 8);
    ofs.write("WAVE", 4);
    ofs.write("fmt ", 4);
    write_int(ofs, 16);// length of format data
    write_short(ofs, 1); // type of format (1 = PCM)
    write_short(ofs, h.channels);
    write_int(ofs, h.sampling_frequency);
    write_int(ofs, h.sampling_frequency * h.bytes_per_sample * h.channels);
    write_short(ofs, h.bytes_per_sample * h.channels);
    write_short(ofs, h.bytes_per_sample * 8);// bits per sample
    ofs.write("data", 4);
    write_int(ofs, h.overallDataSize);
}

const int32_t wavfuncs::time_span_to_seconds(const std::string& timespan)
{
    if (timespan.size() != 8 || timespan[2] != ':' || timespan[5] != ':') throw std::exception("Time span is wrong format");
    int32_t hours = atoi(timespan.substr(0, 2).c_str());
    int32_t mins = atoi(timespan.substr(3, 2).c_str());
    int32_t secs = atoi(timespan.substr(6, 2).c_str());
    return hours * 3600 + mins * 60 + secs;
}
