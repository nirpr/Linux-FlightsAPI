#include <iostream>
#include "slib\flightDB.h"
using namespace std;

int main(int argc, char **argv)
{
    if(argc <= 1)   // print error for didn't send argument to command
        cerr << "Error: arguments didn't send." << endl;
        exit(1);
    FlightDatabase DB;
    string pathDB(argv[0]);
    pathDB += "/flightsDB"; // adding flightsDB directory
    for(int i=1; i < argc;i++)
        DB.load_db(pathDB, argv[i]); 
}
