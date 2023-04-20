#include "full_schedule.h"
using namespace std;

int main(int argc, char** argv)
{
    if (argc <= 1)  // print error for didn't send argument to command
        cerr << "Error: arguments didn't send." << endl;

    FlightDatabase DB;
    string pathDB(argv[0]);
    set<string> airport_name_required;
    getDirectoryFromFile(pathDB);
    pathDB += "\\flightsDB"; // adding flightsDB directory ("\\" for windows and "/" for linux)
    for (int i = 1; i < argc; i++)
    {
        DB.load_db(pathDB, argv[i]);
        airport_name_required.insert(argv[i]); // put toupper
    }
    list<Airport> AirportsRequired = DB.get_flights_by_airport_name(airport_name_required, (int)FlightDatabase::Directions::both);
    cout << "Number of Airports to Report: " << AirportsRequired.size() << endl;
    for (auto& airport : AirportsRequired)
    {
        cout << "Airport " << airport.get_name() << ":" << endl;
        // iterators to Begin of Flights Lists
        list<Flight>::const_iterator itrArv = airport.get_flightsArv().begin();
        list<Flight>::const_iterator itrDst = airport.get_flightsDpt().begin(); 
        // iterators to End of Flights Lists
        list<Flight>::const_iterator itrArvEnd = airport.get_flightsArv().end();
        list<Flight>::const_iterator itrDstEnd = airport.get_flightsDpt().end();
        while(itrArv != itrArvEnd && itrDst != itrDstEnd)
        {
            if( itrArv->get_arrival_time() < itrDst->get_departure_time() )
            {
                printArrivingFlightDetails(*itrArv);
                ++itrArv;
            }
            else
            {
                printDepartingFlightDetails(*itrDst);
                ++itrDst;
            }
        }
        while(itrArv != itrArvEnd)
        {
            printArrivingFlightDetails(*itrArv);
            ++itrArv;
        }
        while(itrDst != itrDstEnd)
        {
            printDepartingFlightDetails(*itrDst);
            ++itrDst;
        }
    }
}

void printDepartingFlightDetails(const Flight& flight)
{
    cout << "Flight #" << flight.get_callsign() << " departing to " <<
    flight.get_destination() << " at " << unix_time_to_date(flight.get_departure_time()) << endl;
}
void printArrivingFlightDetails(const Flight& flight)
{
    cout << "Flight #" << flight.get_callsign() << " arriving from " <<
    flight.get_origin() << " at " << unix_time_to_date(flight.get_arrival_time()) << endl;
}



