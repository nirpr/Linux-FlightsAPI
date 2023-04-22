#include "arrivals.hpp"
using namespace std;

int main(int argc, char **argv)
{
    if(argc <= 1) // print error for didn't send argument to command
        cerr << "Error: arguments didn't send." << endl;
    FlightDatabase DB;
    string pathDB(argv[0]);
    set<string> ICAOcodes;
    getDirectoryFromFile(pathDB);
    pathDB += "/flightsDB"; // adding flightsDB directory
    for(int i=1; i < argc;i++)
    {
        DB.load_db(pathDB, argv[i]); 
        ICAOcodes.insert(argv[i]);
    }
    list<Airport> airportLst = DB.get_flights_by_airport_name(ICAOcodes, (int)FlightDatabase::Directions::arriving);

    for (auto& airport : airportLst)
    {
        list<Flight>::const_iterator itrArv = airport.get_flightsArv().begin();
        list<Flight>::const_iterator itrArvEnd = airport.get_flightsArv().end();
        while(itrArv != itrArvEnd)
        {
            printArrivingFlightDetails(*itrArv);
            ++itrArv;
        }
    }
}


void printArrivingFlightDetails(const Flight& flight)
{
    cout << "Flight #" << flight.get_callsign() << " arriving from " <<
    flight.get_origin() << ", took of at " << unix_time_to_date(flight.get_departure_time()) << "landed at " << unix_time_to_date(flight.get_arrival_time()) << endl;
}