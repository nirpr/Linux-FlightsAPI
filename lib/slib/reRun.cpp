#include "reRun.h"
#include "utility.h"

using namespace std;
namespace fs = std::filesystem;

int reRun(const string &inputs, FlightDatabase &DB, string &errorStrOut)
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
    string infoString;
    try
    {
        DB.load_DB_from_folder(infoString, true);
    }
    catch (const exception &e)
    {
        errorStrOut += e.what() + '\n';
    }
    errorStrOut += infoString; // Append the info String to output infoString
    return stautsBack;
}