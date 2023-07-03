// import libarary
#include "slib/utility.h"
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main()
{
    string serviceToFlights_str = "/tmp/flights_pipe/dbServiceToFlightsService";
    string flightsToService_str = "/tmp/flights_pipe/flightsServiceToDbService";
    int resPipe1 = mkfifo(serviceToFlights_str.c_str(), 0666);
    int resPipe2 = mkfifo(flightsToService_str.c_str(), 0666);
    if (resPipe1 == -1 || resPipe2 == -1) {
        cerr << "Failed to create the named pipe." << endl; // maybe change to errno later
        return EXIT_FAILURE;
    }

    int returnedStatus;
    int serviceToFlights = open(serviceToFlights_str.c_str(), O_WRONLY);
    int flightsToService = open(flightsToService_str.c_str(), O_RDONLY);
    if (serviceToFlights == -1 || flightsToService == -1) {
        cerr << "Failed to open the named pipe." << endl;
        return EXIT_FAILURE;
    }
    try
    {
        // Logic Process - Child Handling
        returnedStatus = LogicProcess(flightsToService, serviceToFlights); // Handle Task and logics from Parent
        pipeCleanUp(flightsToService, serviceToFlights);                                     // Close the pipes between Parent and Child}
        return returnedStatus;     
    }
    catch (const exception &e) // Exit for critical Error that didn't have any specific handle case
    {
        pipeCleanUp(flightsToService, serviceToFlights);                                     // Close the pipes between Parent and Child}
        cerr << e.what() << endl;
    }
}