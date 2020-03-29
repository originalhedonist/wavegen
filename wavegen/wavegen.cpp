#include "pch.h"
#include "../wavelib/headerdata.h"
#include "../wavelib/channel.h"
#include "../wavelib/compositionelement.h"
#include "../wavelib/wavfuncs.h"

using json = nlohmann::json;


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
        int32_t track_length = wavfuncs::time_span_to_seconds(track_length_string);
        std::vector<channel> channels;
        headerdata h(track_length, (int16_t)j["Channels"].size());

        for (auto channeljson : j["Channels"])
        {
            channels.push_back(channel(channeljson, h));
        }

        std::ofstream ofs;
        ofs.open(args[2], std::ios::binary);
        if (!ofs.is_open()) throw std::exception("Unable to write file");

        wavfuncs::write_header(ofs, h);

        compositionelement ce(channels, h);
        ce.calculate();
        ce.start();

        write_wav(ofs, h, (int)channels.size(), ce);

        ofs.close();
        
        return 0;
    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
}
