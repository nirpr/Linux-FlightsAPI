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
    try {
        for (const auto& file_itr : fs::directory_iterator(pathDB))
        {
            string name = file_itr.path().filename().string();
            DB.load_db(pathDB, file_itr.path().filename().string());
            std::cout << file_itr.path().filename().string() << std::endl; // just for debugging
        }
    }
    catch (const fs::filesystem_error& e)
    {
        if (e.code().value() == (int)__std_win_error::_File_not_found || e.code().value() == (int)__std_win_error::_Path_not_found)
            cerr << "Error: FlightDB is not exist, In this case for run this script you should scan for flightsScanner.sh or use reRun script." << endl;
        else if (e.code().value() == (int)__std_win_error::_Access_denied)
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
    for (int i = 1; i <= airplanes_amount; ++i)
    {
        set<Flight> flights = DB.getAirplanes(argv[i]); // need to get only arrivels
        for (auto& flight : flights)
        {
            printAirplaneFlightDetails(flight);
        }
    }
}
