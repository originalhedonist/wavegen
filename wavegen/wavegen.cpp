// wavegen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include "FrequencyFunctionWaveFile.h"
#include "headerdata.h"
#include "wetnessapplier.h"
#include "channel.h"

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
        
        std::ofstream ofs;
        ofs.open(args[2], std::ios::binary);
        if (!ofs.is_open()) throw std::exception("Unable to write file");

        std::string track_length_string = j["TrackLength"];
        int32_t track_length = time_span_to_seconds(track_length_string);
        std::vector<channel> channels;
        headerdata h(track_length, (int16_t)j["Channels"].size());

        for (auto channeljson : j["Channels"])
        {
            channels.push_back(channel(channeljson, h));
        }

        write_header(ofs, h);

        for (int n = 0; n < h.N; n++)
        {
            double t = (double)h.length_seconds * (double)n / h.N;

            for(std::vector<channel>::iterator channelit = channels.begin(); channelit != channels.end(); channelit++)
            {
                double a = 1;

                for(std::vector<FrequencyFunctionWaveFile>::iterator componentit = channelit->components.begin();
                    componentit != channelit->components.end();
                    componentit++)
                {
                    auto athis = componentit->Amplitude(t, n);
                    a *= athis;
                }
                if (a < -1 || a > 1)
                    throw std::exception("Amplitude must be -1 to 1");

                a = channelit->wetness.Amplitude(t, n, a);
                a *= channelit->carrier.Amplitude(t, n);
                int16_t aLs = (int16_t)((a + 1) * ((double)65535 / 2) - 32768);
                write_short(ofs, aLs);
            }
        }

        ofs.close();
        
        return 0;
    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
