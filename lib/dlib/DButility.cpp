#include "DButility.h"
using namespace std;

void getDirectoryFromFile(string &filePath)
{
    const size_t last_slash_index = filePath.rfind('/');
    if (string::npos != last_slash_index)
    {
        filePath = filePath.substr(0, last_slash_index);
    }
}

std::string unix_time_to_date(const time_t &unix_time)
{
    return ctime(&unix_time);
}

int LogicProcess(int readFD, int writeFD) noexcept(false)
{
    bool Running = true;
    try
    {
        FlightDatabase flightDB(false);
        // Opening Message for Parent
        sendMessage(writeFD, "DB service: Hello world, I'm the DB service!\n");
        int opCode = -1;
        while (Running)
        {
            opCode = receiveCodeFromPipe(readFD);
            if (opCode <= (int)Menu::optionStartRange && (int)Menu::optionEndRange <= opCode)
            {
                sendMessage(writeFD, "Error: Unknown option recieved, Program Exited.");
                return EXIT_FAILURE;
            }
            else
            {
                taskHandler(opCode, readFD, writeFD, flightDB);
                if (opCode == (int)Menu::Exit)
                    return EXIT_SUCCESS;
            }
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        sendMessage(writeFD, e.what());
    }
    catch (const runtime_error &e)
    {
        int errorCode = errno;
        if (!(errorCode == EBADF || errorCode == EIO || errorCode == ENOSPC || errorCode == EPIPE)) // Check if failed isn't in pipes
        {
            sendCodeToPipe(writeFD, errorCode);
            sendMessage(writeFD, e.what());
        }
    }
    catch (const exception &e)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void taskHandler(int opCode, int readFD, int writeFD, FlightDatabase &flightDB) noexcept(false)
{
    string args;
    string std_out;
    string errors;
    int statusReturned = -1;

    if (opCode == (int)Menu::arrivingFlightsAirport || opCode == (int)Menu::fullScheduleAirport || opCode == (int)Menu::fullScheduleAircraft || opCode == (int)Menu::updateDB)
        args = receiveMessage(readFD);

    switch (opCode)
    {
        case (int)Menu::updateDB:
            statusReturned = reRun(args, flightDB, errors);
            sendCodeToPipe(writeFD, statusReturned);
            // sendMessage(writeFD, errors);
            break;
        case (int)Menu::arrivingFlightsAirport:
            std_out = arrivals(args, flightDB);
            sendMessage(writeFD, std_out);
            break;
        case (int)Menu::fullScheduleAirport:
            std_out = full_schedule(args, flightDB);
            sendMessage(writeFD, std_out);
            break;
        case (int)Menu::fullScheduleAircraft:
            std_out = airplane(args, flightDB);
            sendMessage(writeFD, std_out);
            break;
        case (int)Menu::zipDB:
        case (int)Menu::Exit:
            try
            {
                flightDB.zipDB();
            }
            catch (const std::runtime_error &e)
            {
                // failed in zipDB
                sendCodeToPipe(writeFD, EXIT_FAILURE);
                sendMessage(writeFD, e.what());
            }
            break;
        default:
            break;
    }
}
