#include "pch.h"
#include "element.h"
#include "../wavelib/wavfuncs.h"

using json = nlohmann::json;

int main(int argc, char** args)
{
    const int CHANNEL_COUNT = 2;

    if (argc != 3) throw std::exception("Wrong number of arguments");

    json j = wavfuncs::read_json(args[1]);
    std::map<int, element> elements;
    std::map<std::string, compositionelement> ces;

    for (auto e : j)
    {
        std::string profile = e["Profile"];
        std::string startstring = e["Start"];
        std::string fadeinstring = e["FadeIn"];
        std::string fadeoutstring = e["FadeOut"];
        int start = wavfuncs::time_span_to_seconds(startstring);
        int fadein = wavfuncs::time_span_to_seconds(fadeinstring);
        int fadeout = wavfuncs::time_span_to_seconds(fadeoutstring);
        if (elements.find(start) != elements.end()) throw std::exception("Cannot have two elements starting at the same time");
        element el(profile, fadein, fadeout);
        elements.insert(std::pair<int, element>(start, el));

        if (ces.find(profile) == ces.end())
        {
            compositionelement ce(wavfuncs::read_json(profile));
            if (ce.channels.size() != CHANNEL_COUNT) throw std::exception("Wrong number of channels");
            ces.insert(std::pair<std::string, compositionelement>(profile, ce));
        }
    }

    for (std::map<std::string, compositionelement>::iterator it = ces.begin();
        it != ces.end(); it++)
    {
        it->second.calculate(); //TODO: multithread it
    }

    int totalLength = 0;
    for (auto e : elements)
    {
        int end = e.first + ces.find(e.second.profile)->second.header.length_seconds;
        totalLength = std::max(end, totalLength);
    }
    totalLength += 60; // minute of silence at the end

    headerdata htot(totalLength, CHANNEL_COUNT);

    std::ofstream ofs;
    ofs.open(args[2], std::ios::trunc | std::ios::binary);
    std::map<int, element>::const_iterator nextStart = elements.begin();

    for (int n = 0; n < htot.N; n++)
    {
        for (int c = 0; c < CHANNEL_COUNT; c++)
        {

        }
    }
    
    ofs.close();

    //try
    //{
    //    if (argc != 3) throw std::exception("Incorrect number of args");

    //    std::ifstream ifs;
    //    ifs.open(args[1], std::ios::in);
    //    if (!ifs.is_open()) throw std::exception("Unable to read file");
    //    json j;
    //    ifs >> j;
    //    ifs.close();

    //    std::cout << "Writing wav using " << args[1] << " to " << args[2] << std::endl;



    //    return 0;
    //}
    //catch (std::exception ex)
    //{
    //    std::cerr << ex.what() << std::endl;
    //    return 1;
    //}
}
