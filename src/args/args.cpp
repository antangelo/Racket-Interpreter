//
// Created by Antonio Abbatangelo on 2019-06-10.
//

#include "args.h"
#include "../interpret/interpret.h"

namespace CLI
{
    void parseCmdArgs(int argc, char *argv[], std::shared_ptr<Expressions::Scope> &globalScope)
    {
        boost::program_options::options_description desc("Opts");
        desc.add_options()
                ("help,h", "Usage info")
                ("require,t", boost::program_options::value<std::vector<boost::filesystem::path>>(), "Require file");

        boost::program_options::variables_map variables;
        try
        {
            boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), variables);

            if (variables.count("help"))
            {
                std::cout << "-h \t\t This help message" << std::endl;
                std::cout << "-t <file> \t Load a file into the interpreter" << std::endl;
            }

            if (variables.count("require"))
            {
                std::vector<boost::filesystem::path> files = variables["require"].as<std::vector<boost::filesystem::path>>();

                for (auto &file : files)
                {
                    boost::filesystem::fstream fileIn(file);

                    Interpreter::repl(fileIn, globalScope, true);
                }
            }
        }
        catch (boost::program_options::error &e)
        {
            std::cerr << e.what() << std::endl;
            std::cerr << "Command parsing error" << std::endl;
        }
    }
}