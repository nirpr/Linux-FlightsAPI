#include "reRun.h"
using namespace std;
namespace fs = std::filesystem;

int reRun(std::string inputs, FlightDatabase &DB)
{
    int stautsBack;
    set<string> airports;
    string command = "./flightScanner.sh ";
    stautsBack = system("rm -r flightsDB"); // save statusBack but not needed

    if (inputs.size() == 0)
    {
        airports = DB.get_airports_names();
        std::string strAirports;
        for (const std::string &str : airports)
            strAirports += str + " ";
        strAirports.pop_back(); // remove the last " "
        command = command + strAirports;
    }
    else
        command = command + inputs; // Adding the inputs to the command

    stautsBack = system(command.c_str());
    DB.load_DB_from_folder(true);
    return stautsBack;
}