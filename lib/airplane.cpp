#include "airplane.h"
using namespace std;

void printAirplaneFlightDetails(const Flight& flight);

int main(int argc, char** argv)
{
    if (argc <= 1)  // print error for didn't send argument to command
        cerr << "Error: arguments didn't send." << endl;

    FlightDatabase DB;
    string pathDB(argv[0]);
    set<string> icao24_required;
    getDirectoryFromFile(pathDB);
    pathDB += "\\flightsDB"; // adding flightsDB directory ("\\" for windows and "/" for linux)
    cout << pathDB << endl;
    for (int i = 1; i < argc; i++)
    {
        DB.load_db(pathDB, argv[i]);
        icao24_required.insert(argv[i]);
    }
    set<Flight> flights = DB.getAirplanes(icao24_required);
    cout << "Number of Airplanes to Report: " << flights.size() << endl;
    cout << "Printing the List of flights for this airplanes:" << endl;
    for (auto& flight : flights)
        printAirplaneFlightDetails(flight);
}

void printAirplaneFlightDetails(const Flight& flight)
{    
    cout << flight.get_icao24() << " " << "departed from" << " " <<
        flight.get_origin() << " " << "at" << " " <<
        unix_time_to_date(flight.get_departure_time()) << " " << "arrived in" << " " <<
        flight.get_destination() << " " << "at" << " " <<
        unix_time_to_date(flight.get_arrival_time()) << endl;
}



