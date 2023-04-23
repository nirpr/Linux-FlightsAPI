#include "utility.h"
using namespace std;

void getDirectoryFromFile(string& filePath)
{
	const size_t last_slash_index = filePath.rfind('/'); // in windows "\\" in linux "/"
	if (string::npos != last_slash_index)
	{
		filePath = filePath.substr(0, last_slash_index);
	}
}

std::string unix_time_to_date(const time_t& unix_time)
{ 
    string time_str(ctime(&unix_time));
    return time_str;
}

void printArrivingFlightDetails(const Flight& flight)
{
    cout << "Flight #" << flight.get_callsign() << " arriving from " <<
        flight.get_origin() << ", took of at " << unix_time_to_date(flight.get_departure_time()) << "landed at " << unix_time_to_date(flight.get_arrival_time()) << endl;
}

void printAirplaneFlightDetails(const Flight& flight)
{
    cout << flight.get_icao24() << " " << "departed from" << " " <<
        flight.get_origin() << " " << "at" << " " <<
        unix_time_to_date(flight.get_departure_time()) << " " << "arrived in" << " " <<
        flight.get_destination() << " " << "at" << " " <<
        unix_time_to_date(flight.get_arrival_time()) << endl;
}

void printDepartingFlightDetailsFS(const Flight& flight)
{
    cout << "Flight #" << flight.get_callsign() << " departing to " <<
        flight.get_destination() << " at " << unix_time_to_date(flight.get_departure_time()) << endl;
}
void printArrivingFlightDetailsFS(const Flight& flight)
{
    cout << "Flight #" << flight.get_callsign() << " arriving from " <<
        flight.get_origin() << " at " << unix_time_to_date(flight.get_arrival_time()) << endl;
}

void printFlightsByOrderFromAirport(const list<Airport>& airportsToPrint)
{
    cout << "Number of Airports to Report: " << airportsToPrint.size() << endl;
    for (auto& airport : airportsToPrint)
    {
        cout << "Airport " << airport.get_name() << ":" << endl;
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
                printArrivingFlightDetailsFS(*itrArv);
                ++itrArv;
            }
            else
            {
                printDepartingFlightDetailsFS(*itrDst);
                ++itrDst;
            }
        }
        while (itrArv != itrArvEnd)
        {
            printArrivingFlightDetailsFS(*itrArv);
            ++itrArv;
        }
        while (itrDst != itrDstEnd)
        {
            printDepartingFlightDetailsFS(*itrDst);
            ++itrDst;
        }
    }
}

void printFlightsArrivalFromAirport(const std::list<Airport>& airportsToPrint)
{
    for (auto& airport : airportsToPrint)
    {
        list<Flight>::const_iterator itrArv = airport.get_flightsArv().begin();
        list<Flight>::const_iterator itrArvEnd = airport.get_flightsArv().end();
        cout << airport.get_name() << ":" << endl;
        while (itrArv != itrArvEnd)
        {
            printArrivingFlightDetails(*itrArv);
            ++itrArv;
        }
    }
}
