#include "full_schedule.h"
#include "DButility.h"

using namespace std;

string full_schedule(string inputs, const FlightDatabase &DB)
{
    string std_out;
    if (inputs.size() <= 0) // print error for didn't send arguments
    {
        std_out += "Error: arguments didn't send.\n";
        return std_out;
    }

    istringstream iss(inputs);
    set<string> airport_name_required;
    string arg;

    // Spliting string to arguments (airplanes codes)
    while (iss >> arg)
        airport_name_required.insert(arg);

    list<Airport> AirportsRequired = DB.get_flights_by_airport_name(airport_name_required, (int)FlightDatabase::Directions::both);

    if (AirportsRequired.size() < 1)
    {
        std_out += "INFO: All the airport that required to print are not valid airports.\n";
        return std_out;
    }

    printFlightsByOrderFromAirportToString(AirportsRequired, std_out);
    return std_out;
}

void printFlightsByOrderFromAirportToString(const list<Airport> &airportsToPrint, string &std_out)
{
    std_out = std_out + "Number of Airports to Report: " + to_string(airportsToPrint.size()) + '\n';
    for (auto &airport : airportsToPrint)
    {
        // iterators to Begin of Flights Lists
        list<Flight>::const_iterator itrArv = airport.get_flightsArv().begin();
        list<Flight>::const_iterator itrDst = airport.get_flightsDpt().begin();
        // iterators to End of Flights Lists
        list<Flight>::const_iterator itrArvEnd = airport.get_flightsArv().end();
        list<Flight>::const_iterator itrDstEnd = airport.get_flightsDpt().end();
        while (itrArv != itrArvEnd && itrDst != itrDstEnd)
        {
            if (itrArv->get_arrival_time() < itrDst->get_departure_time())
            {
                std_out = std_out + "Airport " + airport.get_name() + ": ";
                printArrivingFlightDetailsFSToString(*itrArv, std_out);
                ++itrArv;
            }
            else
            {
                std_out = std_out + "Airport " + airport.get_name() + ": ";
                printDepartingFlightDetailsFSToString(*itrDst, std_out);
                ++itrDst;
            }
        }
        while (itrArv != itrArvEnd)
        {
            std_out = std_out + "Airport " + airport.get_name() + ": ";
            printArrivingFlightDetailsFSToString(*itrArv, std_out);
            ++itrArv;
        }
        while (itrDst != itrDstEnd)
        {
            std_out = std_out + "Airport " + airport.get_name() + ": ";
            printDepartingFlightDetailsFSToString(*itrDst, std_out);
            ++itrDst;
        }
    }
}

void printDepartingFlightDetailsFSToString(const Flight &flight, string &std_out)
{
    std_out = std_out + "Flight #" + flight.get_callsign() + " departing to " + flight.get_destination() + " at " + unix_time_to_date(flight.get_departure_time()) + '\n';
}
void printArrivingFlightDetailsFSToString(const Flight &flight, string &std_out)
{
    std_out = std_out + "Flight #" + flight.get_callsign() + " arriving from " + flight.get_origin() + " at " + unix_time_to_date(flight.get_arrival_time()) + '\n';
}
