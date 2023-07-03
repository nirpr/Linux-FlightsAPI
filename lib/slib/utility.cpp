#include "utility.h"
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
            if (opCode == (int)Menu::Exit || (opCode <= (int)Menu::optionStartRange && (int)Menu::optionEndRange <= opCode))
            {
                if (opCode == (int)Menu::Exit)
                {
                    FlightDatabase::zipDB();
                    Running = false;
                }
                else
                {
                    sendMessage(writeFD, "Error: Unknown option recieved, Program Exited.");
                    return EXIT_FAILURE;
                }
            }
            else
                taskHandler(opCode, readFD, writeFD, flightDB);
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

int UserInterface(int writeFD, int readFD) noexcept(false)
{
    bool Running = true;
    int userChoice, returnedStatus = EXIT_FAILURE; // returnedStatus default fail till Option Handler return something
    // Reading opening message for flights service from DB service
    string openMsg = receiveMessage(readFD);
    cout << openMsg << endl;
    // UI - Main Loop
    while (Running)
    {
        printOptions();
        userChoice = getUserDecision((int)Menu::optionStartRange,
                                        (int)Menu::optionEndRange,
                                        (int)ProgramSettings::MaxInputAttempts);
        returnedStatus = OptionsHandler(userChoice, writeFD, readFD);
        if (userChoice == (int)Menu::Exit)
            Running = false;
    }
    return returnedStatus;
}

void pipeCleanUp(int pipe1, int pipe2)
{
    // Close pipe1 - IO Pipe
    close(pipe1);

    // Close pipe2 - IO Pipe
    close(pipe2);
}

void printOptions()
{
    cout << "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~" << endl;
    cout << "1 - Fetch airports data." << endl;
    cout << "2 - Print airports incoming flights." << endl;
    cout << "3 - Print airports full flights schedule." << endl;
    cout << "4 - Print aircraft full flights schedule." << endl;
    cout << "5 - Zip DB files." << endl;
    cout << "6 - Shutdown." << endl;
    cout << "Please make your choice <1-6>: ";
}

int getUserDecision(int startRange, int endRange, int maxTimes)
{
    int UserDecision;
    cin >> UserDecision;
    while (UserDecision <= startRange || endRange <= UserDecision)
    {
        cout << "Your choice is invalid, please try again <" << startRange - 1 << "-" << endRange - 1 << "> : ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer
        cin >> UserDecision;
    }
    return UserDecision;
}

int OptionsHandler(int OpCode, int writeFD, int readFD) noexcept(false)
{
    string std_out;
    int returnStatus;
    switch (OpCode)
    {
        case (int)Menu::updateDB:
        {
            sendCodeToPipe(writeFD, OpCode);
            string input = getInputFromUser();
            sendMessage(writeFD, input);
            cout << "Waiting for database update." << endl;
            returnStatus = receiveCodeFromPipe(readFD);
            if (returnStatus == EXIT_SUCCESS)
                cout << "Database updated successfully" << endl;
            else if (returnStatus == EXIT_FAILURE)
                cout << "Database update failed." << endl;
            return returnStatus;
        }
        // Same Functionality for 2-4
        case (int)Menu::arrivingFlightsAirport: // Same Logic (NO Break)
        case (int)Menu::fullScheduleAirport:    // Same Logic (NO Break)
        case (int)Menu::fullScheduleAircraft:   // Same Logic (Handle for 1-3 OpCodes)
        {
            sendCodeToPipe(writeFD, OpCode);
            string input = getInputFromUser();
            sendMessage(writeFD, input);
            std_out = receiveMessage(readFD);
            cout << std_out << endl;
            break;
        }

        case (int)Menu::zipDB:
        {
            cout << "Send task to zip flightDB folder to Child Process" << endl;
            sendCodeToPipe(writeFD, OpCode);
            returnStatus = receiveCodeFromPipe(readFD);
            if (returnStatus == EXIT_SUCCESS)
                cout << "Database Zipped successfully" << endl;
            return returnStatus;
        }
        case (int)Menu::Exit:
        {
            //TODO: write stuff
        }
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
            sendMessage(writeFD, errors);
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
            flightDB.zipDB();
            sendCodeToPipe(writeFD, EXIT_SUCCESS);
            break;
        case (int)Menu::Exit:
            flightDB.zipDB();
            sendCodeToPipe(writeFD, EXIT_SUCCESS);
            kill(getpid(), SIGTERM);
            cout << "your'e not suppose to read this" << endl;
        default:
            break;
    }
}

bool sendCodeToPipe(int writePipe, int OpCode) noexcept(false)
{
    ssize_t bytesWritten = write(writePipe, &OpCode, sizeof(OpCode));
    if (bytesWritten == -1)
        throw runtime_error("Failed to write to pipe.");
    return true;
}

int receiveCodeFromPipe(int pipeRead) noexcept(false)
{
    int OpCode = -1; // if -1 returned will be gracefull exit for undefined OpCode
    size_t bytesRead = read(pipeRead, &OpCode, sizeof(OpCode));
    if (bytesRead == -1)
        throw runtime_error("Failed to read task from pipe.");
    return OpCode;
}

string receiveMessage(int pipeRead) noexcept(false)
{
    char buffer[PIPE_BUF];
    int BytesToRead;
    int bytesRead = 0;
    bytesRead = read(pipeRead, &BytesToRead, sizeof(BytesToRead));
    if (bytesRead == -1)
        throw runtime_error("Read message from pipe failed!");

    // Read the strings from the child process
    string stringData;
    char chunk[PIPE_BUF];
    while (BytesToRead > 0)
    {
        int chunkSize = min(PIPE_BUF - 1, BytesToRead);
        int bytesRead = read(pipeRead, chunk, chunkSize);
        if (bytesRead > 0)
        {
            chunk[chunkSize] = '\0';
            stringData.append(chunk); // append string
            BytesToRead -= bytesRead;
        }
        else if (bytesRead == -1)
            throw runtime_error("Read message from pipe failed!");
    }
    return stringData;
}

void sendMessage(int pipeWrite, string message) noexcept(false)
{
    // Sending the size of the message
    int sizesData = message.size();
    if (write(pipeWrite, &sizesData, sizeof(sizesData)) == -1)
        throw runtime_error("Failed to write message into pipe!");

    // Send the string to the pipe
    int remainingSize = message.size();
    int offset = 0;
    // Split the string into smaller chunks if necessary
    while (remainingSize > 0)
    {
        int chunkSize = min(remainingSize, PIPE_BUF);
        if (write(pipeWrite, message.c_str() + offset, chunkSize) == -1)
            throw runtime_error("Failed to write message into pipe!");
        remainingSize -= chunkSize;
        offset += chunkSize;
    }
}

string getInputFromUser()
{
    string arguments;
    string argument;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ignore \n
    while (true)                                         // loop until the user enters an empty string
    {
        cout << "Enter an argument (or press Enter to finish): ";
        getline(cin, argument);

        if (argument.empty()) // if the input is empty, break the loop
            break;
        arguments += argument + " "; // Append the last arguement
    }
    arguments.erase(arguments.find_last_not_of(" \t\n\r\f\v") + 1); // remove whitespaces from end of string
    return arguments;
}
