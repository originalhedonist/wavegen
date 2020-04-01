#include "pch.h"
#include "wavfuncs.h"
#include "compositionreader.h"

void wavfuncs::write_short(std::ostream& os, int16_t i)
{
    os << (unsigned char)(i & 0xff);
    os << (unsigned char)((i >> 8) & 0xff);
}

void wavfuncs::write_int(std::ostream& os, int32_t i)
{
    os << (unsigned char)(i & 0xff);
    os << (unsigned char)((i >> 8) & 0xff);
    os << (unsigned char)((i >> 16) & 0xff);
    os << (unsigned char)((i >> 24) & 0xff);
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

const int32_t wavfuncs::time_span_to_seconds(std::string timespan)
{
    int neg = 1;
    if (timespan.size() > 0 && timespan[0] == '-')
    {
        neg = -1;
        timespan = timespan.substr(1, timespan.size() - 1);
    }

    if (timespan.size() != 8 || timespan[2] != ':' || timespan[5] != ':') throw std::runtime_error("Time span is wrong format");
    int32_t hours = atoi(timespan.substr(0, 2).c_str());
    int32_t mins = atoi(timespan.substr(3, 2).c_str());
    int32_t secs = atoi(timespan.substr(6, 2).c_str());
    return neg * (hours * 3600 + mins * 60 + secs);
}

const std::string wavfuncs::seconds_to_timespan(int32_t seconds)
{
    std::stringstream ss;
    int32_t hours = seconds / (3600);
    ss << std::setfill('0') << std::setw(2) << hours;

	seconds -= (hours * 3600);
	
	int32_t minutes = seconds / 60;
	ss << ":" << std::setfill('0') << std::setw(2) << minutes;
    
	seconds -= (minutes * 60);

	ss << ":" << std::setfill('0') << std::setw(2) << seconds;

	return ss.str();
}

const nlohmann::json wavfuncs::read_json(const std::string& file)
{
    std::ifstream ifs;
    ifs.open(file, std::ios::in);
    if (!ifs.is_open()) throw std::runtime_error("Unable to read file");
    nlohmann::json j;
    ifs >> j;
    ifs.close();
    return j;
}

void wavfuncs::write_double_as_short(std::ostream& ofs, double a)
{
    int aL = ((a + 1) * ((double)65535 / 2) - 32768);
    if (aL > SHRT_MAX) throw std::runtime_error("Exceeded SHRT_MAX");
    if (aL < SHRT_MIN) throw std::runtime_error("Exceeded SHRT_MIN");
    int16_t aLs = (int16_t)aL;
    wavfuncs::write_short(ofs, aLs);
}

void wavfuncs::write_wav(std::ostream& ofs, const headerdata& hOverall, int channels, compositionreader& cr)
{
    for (int n = 0; n < hOverall.N; n++)
    {
        for (int c = 0; c < channels; c++)
        {
            double a = cr.get_next(n, c);
            write_double_as_short(ofs, a);
        }
    }
}
