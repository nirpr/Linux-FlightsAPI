#include "arrivals.h"
using namespace std;

string arrivals(string inputs, const FlightDatabase &DB)
{
    string std_out;
    if (inputs.size() <= 0) // print error for didn't send arguments
    {
        std_out += "Error: arguments didn't send.\n";
        return std_out;
    }
    istringstream iss(inputs);
    set<string> ICAOcodes;
    string arg;

    // Spliting string to arguments
    while (iss >> arg)
        ICAOcodes.insert(arg);

    list<Airport> airportLst = DB.get_flights_by_airport_name(ICAOcodes, (int)FlightDatabase::Directions::arriving);
    printFlightsArrivalFromAirportToString(airportLst, std_out);

    return std_out; // return the final print
}

void printArrivingFlightDetailsToString(const Flight &flight, std::string &std_out)
{
    std_out += "Flight #" + flight.get_callsign() + " arriving from " + flight.get_origin() + ", took of at " + unix_time_to_date(flight.get_departure_time()) + "landed at " + unix_time_to_date(flight.get_arrival_time()) + '\n';
}

void printFlightsArrivalFromAirportToString(const std::list<Airport> &airportsToPrint, std::string &std_out)
{
    for (auto &airport : airportsToPrint)
    {
        list<Flight>::const_iterator itrArv = airport.get_flightsArv().begin();
        list<Flight>::const_iterator itrArvEnd = airport.get_flightsArv().end();
        while (itrArv != itrArvEnd)
        {
            std_out += airport.get_name() + ": ";
            printArrivingFlightDetailsToString(*itrArv, std_out);
            ++itrArv;
        }
    }
}
