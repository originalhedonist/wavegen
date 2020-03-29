#include "pch.h"

using json = nlohmann::json;

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



        return 0;
    }
    catch (std::exception ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}
