#include "arrivals.h"
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
    printFlightsArrivalFromAirport(airportLst);
}
