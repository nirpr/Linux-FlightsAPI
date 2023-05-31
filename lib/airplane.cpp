#include "airplane.h"
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    if (argc <= 1) // print error for didn't send argument to command
    {
        cerr << "Error: arguments didn't send." << endl;
        return EXIT_FAILURE;
    }
    FlightDatabase DB(false);
    string pathDB(argv[0]);
    getDirectoryFromFile(pathDB);
    int airplanes_amount = argc - 1;
    pathDB += "/flightsDB"; // adding flightsDB directory ("\\" for windows and "/" for linux)
    try
    {
        for (const auto &file_itr : fs::directory_iterator(pathDB))
        {
            try
            {
                string name = file_itr.path().filename().string();
                DB.load_db(file_itr.path().filename().string());
            }
            catch (const exception &e)
            {
                cerr << e.what() << endl;
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        if (e.code().value() == (int)std_error::_File_not_found || e.code().value() == (int)std_error::_Path_not_found)
            cerr << "Error: FlightDB is not exist, In this case for run this script you should scan for flightsScanner.sh or use reRun script." << endl;
        else if (e.code().value() == (int)std_error::_Access_denied)
        {
            cerr << "Error: Access Denied to FlightDB, check premissions." << endl;
            return false;
        }
        else
        {
            cerr << "Error: Unknown error in reading data from Database" << endl;
        }
        return EXIT_FAILURE;
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    // Printing the Flights
    printFlightsByAirplanes(DB, argv, airplanes_amount);
}

void printAirplaneFlightDetails(const Flight &flight)
{
    cout << flight.get_icao24() << " "
         << "departed from"
         << " " << flight.get_origin() << " "
         << "at"
         << " " << unix_time_to_date(flight.get_departure_time()) << " "
         << "arrived in"
         << " " << flight.get_destination() << " "
         << "at"
         << " " << unix_time_to_date(flight.get_arrival_time()) << endl;
}

void printFlightsByAirplanes(const FlightDatabase &DB, char **argv, int airplanes_amount)
{
    for (int i = 1; i <= airplanes_amount; ++i)
    {
        set<Flight> flights = DB.getAirplanes(argv[i]); // need to get only arrivels
        for (auto &flight : flights)
        {
            printAirplaneFlightDetails(flight);
        }
    }
}