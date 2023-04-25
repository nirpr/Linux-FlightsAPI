#include "full_schedule.h"
using namespace std;

int main(int argc, char** argv)
{
    if (argc <= 1)  // print error for didn't send argument to command
    {
        cerr << "Error: arguments didn't send." << endl;
        return EXIT_FAILURE;
    }
    FlightDatabase DB;
    string pathDB(argv[0]);
    getDirectoryFromFile(pathDB);
    pathDB += "/flightsDB";
    set<string> airport_name_required;
    for (int i = 1; i < argc; i++)
    {
        try 
        {
            if( airport_name_required.insert(argv[i]).second ) // try to load the airport only if this airport not in the set
                DB.load_db(pathDB, argv[i]);
        }
        catch(const exception& e)
        {
            cerr << e.what() << endl;
        }
    }
    if (airport_name_required.size() < 1)
    {
        cout << endl << "All the airport that required to print are not valid airports." << endl;
        return EXIT_SUCCESS;
    }
    list<Airport> AirportsRequired = DB.get_flights_by_airport_name(airport_name_required, (int)FlightDatabase::Directions::both);
    printFlightsByOrderFromAirport(AirportsRequired);
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



