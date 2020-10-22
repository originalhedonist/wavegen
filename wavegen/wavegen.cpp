#include "pch.h"
#include "../wavelib/headerdata.h"
#include "../wavelib/channel.h"
#include "../wavelib/compositionelement.h"
#include "../wavelib/compositionreader.h"
#include "../wavelib/wavfuncs.h"

using json = nlohmann::json;

int main(int argc, char** args)
{
    try
    {
        if (argc != 3) throw std::runtime_error("Incorrect number of args");


        std::cout << "Writing wav using " << args[1] << " to " << args[2] << std::endl;

        std::ofstream ofs;
        ofs.open(args[2], std::ios::binary);
        if (!ofs.is_open()) throw std::runtime_error("Unable to write file");

        std::map<std::string, double> constants; // none defined yet - these are only used by symphgen
        compositionelement ce(wavfuncs::read_json(args[1]), constants);

        wavfuncs::write_header(ofs, ce.header);

        ce.calculate();

        compositionreader cr(ce);

        wavfuncs::write_wav(ofs, ce.header, ce.channels.size(), cr);

        ofs.close();

        remove(ce.tempfilename.c_str());
        
        return 0;
    }
    catch (std::runtime_error ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
}
