#include "slib/utility.h"
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    string airports;
    string command = "/bin/bash -c ./flightDB.sh";
    if (argc == 1)
    {
        string pathDB(argv[0]);
        getDirectoryFromFile(pathDB);
        int airplanes_amount = argc - 1;
        pathDB += "/flightsDB"; // adding flightsDB directory ("\\" for windows and "/" for linux)
        for (const auto& file_itr : fs::directory_iterator(pathDB))
        {
            airports = airports + ' ' + file_itr.path().filename().string();
        }
    }
    else
    {
        for (int i = 1; i < argc; ++i)
        {
            airports += ' ';
            airports += argv[i];
        }
    }
    command =  command + ' ' +  airports;
    system(command.c_str());
}