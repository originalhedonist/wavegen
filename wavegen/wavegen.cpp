// wavegen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include "FrequencyFunctionWaveFile.h"
#include "headerdata.h"
#include "channel.h"
#include "compositionelement.h"

using json = nlohmann::json;

void write_short(std::ostream& os, int16_t i)
{
    os << (BYTE)(i & 0xff);
    os << (BYTE)((i >> 8) & 0xff);
}

void write_int(std::ostream& os, int32_t i)
{
    os << (BYTE)(i & 0xff);
    os << (BYTE)((i >> 8) & 0xff);
    os << (BYTE)((i >> 16) & 0xff);
    os << (BYTE)((i >> 24) & 0xff);
}

void write_header(std::ofstream& ofs, const headerdata& h)
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

const int32_t time_span_to_seconds(const std::string& timespan)
{
    if (timespan.size() != 8 || timespan[2] != ':' || timespan[5] != ':') throw std::exception("Time span is wrong format");
    int32_t hours = atoi(timespan.substr(0, 2).c_str());
    int32_t mins = atoi(timespan.substr(3, 2).c_str());
    int32_t secs = atoi(timespan.substr(6, 2).c_str());
    return hours * 3600 + mins * 60 + secs;
}

void write_wav(std::ostream& ofs, const headerdata& hOverall, int channels, compositionelement& ce);

int main(int argc, char** args)
{
    try
    {
        if (argc != 3) throw std::exception("Incorrect number of args");

        std::ifstream ifs;
        ifs.open(args[1], std::ios::in);
        if (!ifs.is_open()) throw std::exception("Unable to read file");
        json j;
        ifs >> j;
        ifs.close();

        std::cout << "Writing wav using " << args[1] << " to " << args[2] << std::endl;

        std::string track_length_string = j["TrackLength"];
        int32_t track_length = time_span_to_seconds(track_length_string);
        std::vector<channel> channels;
        headerdata h(track_length, (int16_t)j["Channels"].size());

        for (auto channeljson : j["Channels"])
        {
            channels.push_back(channel(channeljson, h));
        }

        std::ofstream ofs;
        ofs.open(args[2], std::ios::binary);
        if (!ofs.is_open()) throw std::exception("Unable to write file");

        write_header(ofs, h);

        compositionelement ce(channels, h);
        ce.calculate();
        ce.start();

        write_wav(ofs, h, channels.size(), ce);

        ofs.close();
        
        return 0;
    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
}
