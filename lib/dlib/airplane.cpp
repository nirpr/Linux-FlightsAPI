#include "airplane.h"
#include "DButility.h"

using namespace std;
namespace fs = std::filesystem;

string airplane(string inputs, const FlightDatabase &DB)
{
    string std_out;
    string std_Error;
    if (inputs.size() <= 0) // print error for didn't send arguments
    {
        std_out += "Error: arguments didn't send.\n";
        return std_out;
    }

    istringstream iss(inputs);
    set<string> airplanes;
    string arg;

    // Spliting string to arguments (airplanes codes)
    while (iss >> arg)
        airplanes.insert(arg);

    printFlightsByAirplanesToString(DB, airplanes, std_out, std_Error);
    std_out = std_out + std_Error;
    return std_out;
}

void printAirplaneFlightDetailsToString(const Flight &flight, string &sdt_out)
{
    sdt_out = sdt_out + flight.get_icao24() + " " + "departed from" + " " + flight.get_origin() + " " + "at" + " " + unix_time_to_date(flight.get_departure_time()) + " " + "arrived in" + " " + flight.get_destination() + " " + "at" + " " + unix_time_to_date(flight.get_arrival_time()) + '\n';

    return;
}

void printFlightsByAirplanesToString(const FlightDatabase &DB, set<string> airplanes, string &std_out, string &std_Error)
{
    for (const auto &airplane : airplanes)
    {
        set<Flight> flights = DB.getAirplanes(airplane); // need to get only arrivels
        if (flights.size() == 0)
            std_Error = std_Error + "INFO: Airplane " + airplane + " is not in the Database.\n";

        for (auto &flight : flights)
            printAirplaneFlightDetailsToString(flight, std_out);
    }
}