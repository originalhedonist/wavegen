#include "pch.h"
#include "../wavelib/headerdata.h"
#include "../wavelib/channel.h"
#include "../wavelib/compositionelement.h"
#include "../wavelib/compositionreader.h"
#include "../wavelib/wavfuncs.h"

using json = nlohmann::json;


void write_wav(std::ostream& ofs, const headerdata& hOverall, int channels, compositionelement& ce);

int main(int argc, char** args)
{
    try
    {
        if (argc != 3) throw std::exception("Incorrect number of args");


        std::cout << "Writing wav using " << args[1] << " to " << args[2] << std::endl;


        std::ofstream ofs;
        ofs.open(args[2], std::ios::binary);
        if (!ofs.is_open()) throw std::exception("Unable to write file");

        compositionelement ce(wavfuncs::read_json(args[1]));

        wavfuncs::write_header(ofs, ce.header);

        ce.calculate();

        compositionreader cr(ce);

        wavfuncs::write_wav(ofs, ce.header, ce.channels.size(), ce);

        ofs.close();
        
        return 0;
    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
}
