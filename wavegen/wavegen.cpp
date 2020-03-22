// wavegen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include "FrequencyFunctionWaveFile.h"
#include "c.h"

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

void write_header(std::ofstream& ofs)
{
    ofs.write("RIFF", 4);
    write_int(ofs, c::overallFileSize - 8);
    ofs.write("WAVE", 4);
    ofs.write("fmt ", 4);
    write_int(ofs, 16);// length of format data
    write_short(ofs, 1); // type of format (1 = PCM)
    write_short(ofs, c::channels);
    write_int(ofs, c::sampling_frequency);
    write_int(ofs, c::sampling_frequency * c::bytes_per_sample * c::channels);
    write_short(ofs, c::bytes_per_sample * c::channels);
    write_short(ofs, c::bytes_per_sample * 8);// bits per sample
    ofs.write("data", 4);
    write_int(ofs, c::overallDataSize);
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
        write_header(ofs);

        FrequencyFunctionWaveFile l("800 + 200*n/N", "sin(x)");
        FrequencyFunctionWaveFile r("800 - 200*n/N", "cos(x)");
        for (int n = 0; n < c::N; n++)
        {
            double t = (double)c::length_seconds * (double)n / c::N;

            double aLd = l.Amplitude(t, n);
            if (aLd < -1 || aLd > 1)
                throw std::exception("Amplitude must be -1 to 1");
            int16_t aLs = (int16_t)((aLd + 1) * ((double)65535 / 2) - 32768);
            write_short(ofs, aLs);

            double aRd = r.Amplitude(t, n);
            if (aRd < -1 || aRd > 1)
                throw std::exception("Amplitude must be -1 to 1");
            int16_t aRs = (int16_t)((aRd + 1) * ((double)65535 / 2) - 32768);
            write_short(ofs, aRs);
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
