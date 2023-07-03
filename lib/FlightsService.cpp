// import libarary
#include "slib/utility.h"
using namespace std;


// import libarary
#include "slib/utility.h"
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main()
{
    string serviceToFlights_str = "/tmp/flights_pipe/dbServiceToFlightsService";
    string flightsToService_str = "/tmp/flights_pipe/flightsServiceToDbService";

    int returnedStatus;
    int serviceToFlights = open(serviceToFlights_str.c_str(), O_RDONLY);
    int flightsToService = open(flightsToService_str.c_str(), O_WRONLY);
    if (serviceToFlights == -1 || flightsToService == -1) {
        cerr << "Failed to open the named pipe." << endl;
        return EXIT_FAILURE;
    }
    try
    {
        returnedStatus = UserInterface(flightsToService, serviceToFlights); // Handle UI
        pipeCleanUp(flightsToService, serviceToFlights);                                         // Close the pipes between Parent and Child}
        return returnedStatus;
    
    }
    catch (const exception &e) // Exit for critical Error that didn't have any specific handle case
    {
        pipeCleanUp(flightsToService, serviceToFlights);                                     // Close the pipes between Parent and Child}
        cerr << e.what() << endl;
    }
}