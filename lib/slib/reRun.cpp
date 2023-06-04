#include "utility.h"
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

int rerun(std::string inputs, FlightDatabase& DB)
{
    bool rerun = true;
    set<string> airports;
    string command = "./flightScanner.sh";
    if (inputs.size() == 0)
    {
        airports = DB.get_airports_names();
    }
    else
    {
        system("rm -r flightsDB");
        system("rm flightsDB.zip")
        command = command + inputs;
        system(command.c_str());
        DB.load_db(rerun)
        return EXIT_SUCCESS;
    }
}