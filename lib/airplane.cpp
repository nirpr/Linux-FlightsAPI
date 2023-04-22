#include "airplane.h"
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    if (argc <= 1)   // print error for didn't send argument to command
    {
        cerr << "Error: arguments didn't send." << endl;
        exit(1);
    }
    FlightDatabase DB;
    string pathDB(argv[0]);
    getDirectoryFromFile(pathDB);
    int airplanes_amount = argc - 1;
    pathDB += "/flightsDB"; // adding flightsDB directory ("\\" for windows and "/" for linux)
    for (const auto& file_itr : fs::directory_iterator(pathDB))
    {
        string name = file_itr.path().filename().string();
        DB.load_db(pathDB, file_itr.path().filename().string());
        std::cout << file_itr.path().filename().string() << std::endl; // just for debugging
    }

    for (int i = 1; i <= airplanes_amount; ++i)
    {
        set<Flight> flights = DB.getAirplanes(argv[i]); // need to get only arrivels
        for (auto& flight : flights)
        {
            printAirplaneFlightDetails(flight);
        }
    }
}


void printAirplaneFlightDetails(const Flight& flight)
{
    cout << flight.get_icao24() << " " << "departed from" << " " <<
        flight.get_origin() << " " << "at" << " " <<
        unix_time_to_date(flight.get_departure_time()) << " " << "arrived in" << " " <<
        flight.get_destination() << " " << "at" << " " <<
        unix_time_to_date(flight.get_arrival_time()) << endl;
}