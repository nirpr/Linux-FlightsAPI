#include "arrivals.h"
using namespace std;

string arrivals(string inputs, const FlightDatabase &DB)
{
    string str_out;
    if (inputs.size() > 0) // print error for didn't send argument to command
    {
        str_out += "Error: arguments didn't send.\n";
        return str_out;
    }
    std::istringstream iss(inputs);
    set<string> ICAOcodes;
    std::string word;
    // Process each word individually
    while (iss >> word)
    {
        ICAOcodes.insert(word);
    }
    list<Airport> airportLst = DB.get_flights_by_airport_name(ICAOcodes, (int)FlightDatabase::Directions::arriving);
    printFlightsArrivalFromAirport(airportLst);
}

void printArrivingFlightDetails(const Flight &flight)
{
    cout << "Flight #" << flight.get_callsign() << " arriving from " << flight.get_origin() << ", took of at " << unix_time_to_date(flight.get_departure_time()) << "landed at " << unix_time_to_date(flight.get_arrival_time()) << endl;
}

void printFlightsArrivalFromAirport(const std::list<Airport> &airportsToPrint)
{
    for (auto &airport : airportsToPrint)
    {
        list<Flight>::const_iterator itrArv = airport.get_flightsArv().begin();
        list<Flight>::const_iterator itrArvEnd = airport.get_flightsArv().end();
        while (itrArv != itrArvEnd)
        {
            cout << airport.get_name() << ": ";
            printArrivingFlightDetails(*itrArv);
            ++itrArv;
        }
    }
}
