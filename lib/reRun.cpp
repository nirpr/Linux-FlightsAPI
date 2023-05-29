#include "slib/utility.h"
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    string airports;
    string command = "./flightScanner.sh";
    if (argc == 1)
    {
        string pathDB(argv[0]);
        getDirectoryFromFile(pathDB);
        int airplanes_amount = argc - 1;
        pathDB += "/flightsDB"; // adding flightsDB directory
        try
        {
            for (const auto &file_itr : fs::directory_iterator(pathDB))
            {
                airports = airports + ' ' + file_itr.path().filename().string();
            }
        }
        catch (const fs::filesystem_error &e)
        {
            if (e.code().value() == (int)std_error::_File_not_found || e.code().value() == (int)std_error::_Path_not_found)
                cerr << "Error: FlightDB is not exist, In this case for run this script and load database need to enter paramaters." << endl;
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
            cerr << e.what();
            return EXIT_FAILURE;
        }
    }
    else
    {
        system("rm -r flightsDB");
        for (int i = 1; i < argc; ++i)
        {
            airports += ' ';
            airports += argv[i];
        }
    }
    command = command + airports;
    system(command.c_str());
    return EXIT_SUCCESS;
}
