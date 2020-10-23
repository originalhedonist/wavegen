#include "pch.h"
#include "element.h"
#include "activeelement.h"
#include "../wavelib/wavfuncs.h"

using json = nlohmann::json;

int main(int argc, char** args)
{
    try
    {
        //const int CHANNEL_COUNT = 2;

        int CHANNEL_COUNT = -1;
        if (argc != 3) throw std::runtime_error("Wrong number of arguments");

        json j = wavfuncs::read_json(args[1]);
        std::map<int, element> elements;
        
        std::map<profilekey, compositionelement*> ces;

        int pos = 0;
        for (auto e : j)
        {
            std::string profile = e["Profile"];
            std::string startstring = e["Start"];
            std::string fadeinstring = e["FadeIn"];
            std::string fadeoutstring = e["FadeOut"];
            double attenuation = 1;
            if(e.contains("Attenuation"))
            {
                e["Attenuation"].get_to(attenuation);
            }

            int relstart = wavfuncs::time_span_to_seconds(startstring);
            int start = pos + relstart;

			std::cout << profile << " starts at " << wavfuncs::seconds_to_timespan(start) << std::endl;
            int fadein = wavfuncs::time_span_to_seconds(fadeinstring);
            int fadeout = wavfuncs::time_span_to_seconds(fadeoutstring);
            if (elements.find(start) != elements.end()) throw std::runtime_error("Cannot have two elements starting at the same time");


            std::map<std::string, double> constants;
            
            for(nlohmann::json::const_iterator it = e.begin(); it != e.end(); it++)
            {
                if(strcmp(it.key().substr(0,1).c_str(), ":") == 0)
                {
                    auto constname = it.key().substr(1);
                    double constval = 0;
                    it.value().get_to(constval);
                    constants.insert(std::pair<std::string, double>(constname, constval));
                }
            }

            profilekey pk(profile, constants);
            element el(pk, fadein, fadeout, attenuation);
            elements.insert(std::pair<int, element>(start, el));

            auto cex = ces.find(pk);
            if (cex == ces.end())
            {
                compositionelement* ce = new compositionelement(wavfuncs::read_json(profile), constants);
                if(CHANNEL_COUNT == -1) // they can have any number of channels, but they must all have the same number, once CHANNEL_COUNT is first set, they must all have that number of channels
                {
                    CHANNEL_COUNT = ce->channels.size();
                }
                else if (ce->channels.size() != CHANNEL_COUNT)
                {
                    throw std::runtime_error("Wrong number of channels");
                }
                ces.insert(std::pair<profilekey, compositionelement*>(pk, ce));
                pos = start + ce->header.length_seconds;
            }
            else
            {
                pos = start + cex->second->header.length_seconds;
            }
        }

        std::vector<std::thread> calcthreads;

        for (std::map<profilekey, compositionelement*>::iterator it = ces.begin();
            it != ces.end(); it++)
        {
            calcthreads.push_back(std::thread(&compositionelement::calculate, it->second));
        }

        for (std::vector<std::thread>::iterator it = calcthreads.begin();
            it != calcthreads.end();
            it++)
        {
            it->join();
        }

        int totalLength = 0;
        for (auto e : elements)
        {
            int end = e.first + ces.find(e.second.profile)->second->header.length_seconds;
            totalLength = std::max(end, totalLength);
        }
        totalLength += 60; // minute of silence at the end

        headerdata htot(totalLength, CHANNEL_COUNT);

        std::ofstream ofs;

        ofs.open(args[2], std::ios::trunc | std::ios::binary);
        if (!ofs.is_open()) throw std::runtime_error("Could not open output file");
        wavfuncs::write_header(ofs, htot);

        std::map<int, element>::const_iterator nextStart = elements.begin();
        int nextStartn = nextStart->first * htot.sampling_frequency;

        std::vector<activeelement> active;

        for (int n = 0; n < htot.N; n++)
        {
            for (int c = 0; c < CHANNEL_COUNT; c++)
            {
                //new one becoming active
                if (n == nextStartn && c == 0)
                {
                    compositionreader* cr = new compositionreader(*ces.find(nextStart->second.profile)->second);
                    active.push_back(activeelement(n, nextStart->second, cr));
                    
                    nextStart++;
                    if (nextStart == elements.end())
                        nextStartn = -1;
                    else
                        nextStartn = nextStart->first * htot.sampling_frequency;
                }

                double a = 0;
                for (std::vector<activeelement>::const_iterator it = active.begin();
                    it != active.end(); // don't it++ in order to avoid double-increment if delete
                    )
                {
                    int nrel = n - it->nStart;
                    double aThis = it->ce->get_next(nrel, c);
                
                    if(isnan(aThis) || aThis < -1 || aThis > 1)
                    {
                        std::cout << "aThis=" << aThis << std::endl;
                        throw std::runtime_error("aThis out of range or NaN");
                    }

                    double fadeatt = it->attenuation(nrel);
                    a += aThis*fadeatt;
                    if (a > 1 || a < -1) throw std::runtime_error("Additive amplitude outside range");

                    if (it->ce->is_complete())
                    {
                        delete it->ce;
                        it = active.erase(it);
                        if (it == active.end())
                        {
                            break;
                        }
                    }
                    else it++;
                }
                wavfuncs::write_double_as_short(ofs, a);
            }
        }

        ofs.close();
        for (auto e : ces)
        {
            remove(e.second->tempfilename.c_str());
            delete e.second;
        }
    }
    catch (std::runtime_error ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

}
