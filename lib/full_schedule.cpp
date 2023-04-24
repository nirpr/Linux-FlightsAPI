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
    set<string> airport_name_required;
    getDirectoryFromFile(pathDB);
    pathDB += "/flightsDB"; // adding flightsDB directory ("\\" for windows and "/" for linux)
    for (int i = 1; i < argc; i++)
    {
        if( DB.load_db(pathDB, argv[i]) )
            airport_name_required.insert(argv[i]); // put toupper
    }
    if (airport_name_required.size() < 1)
    {
        cout << endl << "All the airport that required to print are not valid airports." << endl;
        return EXIT_SUCCESS;
    }
    list<Airport> AirportsRequired = DB.get_flights_by_airport_name(airport_name_required, (int)FlightDatabase::Directions::both);
    printFlightsByOrderFromAirport(AirportsRequired);
}





