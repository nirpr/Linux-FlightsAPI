// import libarary
#include "flib/flightsUtility.h"
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
using namespace std;


void sigpipeHandler(int signal) {
    std::cerr << "Caught SIGPIPE signal. Writing to closed pipe." << std::endl;
}

int main()
{
    signal(SIGPIPE, sigpipeHandler);

    string serviceToFlights_str = "/tmp/flights_pipe/dbServiceToFlightsService";
    string flightsToService_str = "/tmp/flights_pipe/flightsServiceToDbService";

    int returnedStatus;
    int serviceToFlights = open(serviceToFlights_str.c_str(), O_RDONLY);
    int flightsToService = open(flightsToService_str.c_str(), O_WRONLY);
    if (serviceToFlights == -1 || flightsToService == -1)
    {
        cerr << "Failed to open the named pipe." << endl;
        return EXIT_FAILURE;
    }
    try
    {
        returnedStatus = UserInterface(flightsToService, serviceToFlights); // Handle UI
        pipeCleanUp(flightsToService, serviceToFlights);                    // Close the pipes between Parent and Child
        return returnedStatus;
        cout << "This line should never be printed !" << endl;
    }
    catch (const exception &e) // Exit for critical Error that didn't have any specific handle case
    {
        pipeCleanUp(flightsToService, serviceToFlights); // Close the pipes between Parent and Child}
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}