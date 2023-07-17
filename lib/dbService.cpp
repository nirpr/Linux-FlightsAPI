// import libarary
#include "dlib/DButility.h"
#include <fcntl.h>
#include <unistd.h>

using namespace std;
#include <signal.h>

void sigpipeHandler(int signal) {
    std::cerr << "Caught SIGPIPE signal. Writing to closed pipe." << std::endl;
}

int main()
{
    signal(SIGPIPE, sigpipeHandler);
    string serviceToFlights_str = "/tmp/flights_pipe/dbServiceToFlightsService";
    string flightsToService_str = "/tmp/flights_pipe/flightsServiceToDbService";
    int resPipe1 = mkfifo(serviceToFlights_str.c_str(), 0666);
    int resPipe2 = mkfifo(flightsToService_str.c_str(), 0666);
    if ((resPipe1 == -1 || resPipe2 == -1) && errno != EEXIST)
    {
        cerr << "Failed to create the named pipe." << endl;
        return EXIT_FAILURE;
    }

    int returnedStatus;
    int serviceToFlights = open(serviceToFlights_str.c_str(), O_WRONLY);
    int flightsToService = open(flightsToService_str.c_str(), O_RDONLY);
    if (serviceToFlights == -1 || flightsToService == -1)
    {
        cerr << "Failed to open the named pipe." << endl;
        return EXIT_FAILURE;
    }
    try
    {
        // Logic Process - DB Services Handling
        returnedStatus = LogicProcess(flightsToService, serviceToFlights); // Handle Task and logics from flight Service
        pipeCleanUp(flightsToService, serviceToFlights);                   // Close the pipes between Containers FlightService and dbService
        return returnedStatus;
        cout << "This line should never be printed !" << endl;
    }
    catch (const exception &e) // Exit for critical Error that didn't have any specific handle case
    {
        pipeCleanUp(flightsToService, serviceToFlights); // Close the pipes between Containers FlightService and dbService
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}