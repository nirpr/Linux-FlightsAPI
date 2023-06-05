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

int LogicProcess(int parentToChild, int childToParent) noexcept(false)
{
    bool Running = true;
    try
    {
        FlightDatabase flightDB(true);
        // Opening Message for Parent
        sendMessage(childToParent, "Child Process: Hello world, I'm the CHILD!\n");
        int opCode = -1;
        while (Running)
        {
            opCode = receiveCodeFromPipe(parentToChild);
            if (opCode == (int)Menu::Exit || (opCode <= (int)Menu::optionStartRange && (int)Menu::optionEndRange <= opCode))
            {
                if (opCode == (int)Menu::Exit)
                {
                    FlightDatabase::zipDB();
                    Running = false;
                }
                else
                {
                    sendMessage(childToParent, "Error: Unknown option recieved to Child, Program Exited.");
                    return EXIT_FAILURE;
                }
            }
            else
                taskHandler(opCode, parentToChild, childToParent, flightDB);
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        sendMessage(childToParent, e.what());
    }
    catch (const runtime_error &e)
    {
        int errorCode = errno;
        if (!(errorCode == EBADF || errorCode == EIO || errorCode == ENOSPC || errorCode == EPIPE)) // Check if failed isn't in pipes
        {
            sendCodeToPipe(childToParent, errorCode);
            sendMessage(childToParent, e.what());
        }
    }
    catch (const exception &e)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int UserInterface(int parentToChild, int childToParent) noexcept(false)
{
    bool Running = true;
    int userChoice, returnedStatus = EXIT_FAILURE; // returnedStatus default fail till Option Handler return something
    // Reading opening message for Parent from Child
    string openMsg = receiveMessage(childToParent);
    cout << openMsg << endl;
    // UI - Main Loop
    while (Running)
    {
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == -1)
        {
            cerr << "Error occurred while waiting for child process" << endl;
            return EXIT_FAILURE;
        }
        else if (result == 0)
        {
            printOptions();
            userChoice = getUserDecision((int)Menu::optionStartRange,
                                         (int)Menu::optionEndRange,
                                         (int)ProgramSettings::MaxInputAttempts);
            returnedStatus = OptionsHandler(userChoice, parentToChild, childToParent);
            if (userChoice == (int)Menu::Exit)
                Running = false;
        }
        else
        {
            if (WIFEXITED(status)) // Child process exited normaly
            {
                int exitStatus = WEXITSTATUS(status);
                cout << "Child process exited with status: " << exitStatus << endl;
            }
            else if (WIFSIGNALED(status)) // Child process terminated
            {
                int signalNumber = WTERMSIG(status);
                cout << "Child process terminated due to signal: " << signalNumber << endl;
            }
        }
    }
    return returnedStatus;
}

void pipeCleanUp(int parentToChild[2], int childToParent[2])
{
    // Close Parent to Child - IO Pipe
    close(parentToChild[READ_END]);
    close(parentToChild[WRITE_END]);

    // Close Child to Parent - IO Pipe
    close(childToParent[READ_END]);
    close(childToParent[WRITE_END]);
}

void signalHandlerParent(int signal_number)
{
    switch (signal_number)
    {
        case SIGINT:
            cout << endl
                 << "Received SIGINT signal" << endl;
            gracefulExit(); // this is the child's pid
            exit(SIGINT);
            break;
        default:
            cout << "Received unknown signal" << endl;
    }
}

void signalHandlerChild(int signal_number)
{
    switch (signal_number)
    {
        case SIGINT: // Ignoring SIGINT from child's prespective
            break;
        case SIGUSR1:
            FlightDatabase::zipDB();
            exit(SIGUSR1);
            break;
        default:
            break;
    }
}

void printOptions()
{
    cout << "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~" << endl;
    cout << "1 - Fetch airports incoming flights." << endl;
    cout << "2 - Fetch airports full flights schedule." << endl;
    cout << "3 - Fetch aircraft full flights schedule." << endl;
    cout << "4 - Update DB." << endl;
    cout << "5 - Zip DB files." << endl;
    cout << "6 - Get child process PID." << endl;
    cout << "7 - Exit." << endl;
    cout << "Please make your choice <1-7>: ";
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

int OptionsHandler(int OpCode, int parentToChild, int childToParent) noexcept(false)
{
    string std_out;
    int returnStatus;
    switch (OpCode)
    {
        // Same Functionality for 1-3
        case (int)Menu::arrivingFlightsAirport: // Same Logic (NO Break)
        case (int)Menu::fullScheduleAirport:    // Same Logic (NO Break)
        case (int)Menu::fullScheduleAircraft:   // Same Logic (Handle for 1-3 OpCodes)
        {
            sendCodeToPipe(parentToChild, OpCode);
            string input = getInputFromUser();
            sendMessage(parentToChild, input);
            std_out = receiveMessage(childToParent);
            cout << std_out << endl;
            break;
        }
        case (int)Menu::updateDB:
        {
            sendCodeToPipe(parentToChild, OpCode);
            string input = getInputFromUser();
            sendMessage(parentToChild, input);
            cout << "Waiting for database update." << endl;
            returnStatus = receiveCodeFromPipe(childToParent);
            if (returnStatus == EXIT_SUCCESS)
                cout << "Database updated successfully" << endl;
            else if (returnStatus == EXIT_FAILURE)
                cout << "Database update failed." << endl;
            return returnStatus;
        }
        case (int)Menu::zipDB:
        {
            cout << "Send task to zip flightDB folder to Child Process" << endl;
            sendCodeToPipe(parentToChild, OpCode);
            returnStatus = receiveCodeFromPipe(childToParent);
            if (returnStatus == EXIT_SUCCESS)
                cout << "Database Zipped successfully" << endl;
            return returnStatus;
        }
        case (int)Menu::childPID:
        {
            cout << "The Process Child's ID: " << pid << endl;
            break;
        }
        case (int)Menu::Exit:
        {
            int status;
            pid_t terminated_pid;
            gracefulExit();
            terminated_pid = waitpid(pid, &status, WUNTRACED); // wait for exit status from child process

            // Printing process exit status
            if (WIFEXITED(status))
                cout << "Process with PID " << terminated_pid << " terminated with exit status: " << WEXITSTATUS(status) << endl;
            else if (WIFSTOPPED(status))
                cout << "Process with PID" << terminated_pid << " stopped" << endl;
            if (WEXITSTATUS(status) == SIGUSR1)
            {
                cout << "Program Exited Successfully." << endl;
                return SIGINT;
            }
            break;
        }
    }
    return EXIT_SUCCESS;
}

void taskHandler(int opCode, int parentToChild, int childToParent, FlightDatabase &flightDB) noexcept(false)
{
    string args;
    string std_out;
    string errors;
    int statusReturned = -1;

    if (opCode == (int)Menu::arrivingFlightsAirport || opCode == (int)Menu::fullScheduleAirport || opCode == (int)Menu::fullScheduleAircraft || opCode == (int)Menu::updateDB)
        args = receiveMessage(parentToChild);

    switch (opCode)
    {
        case (int)Menu::arrivingFlightsAirport:
            std_out = arrivals(args, flightDB);
            sendMessage(childToParent, std_out);
            break;
        case (int)Menu::fullScheduleAirport:
            std_out = full_schedule(args, flightDB);
            sendMessage(childToParent, std_out);
            break;
        case (int)Menu::fullScheduleAircraft:
            std_out = airplane(args, flightDB);
            sendMessage(childToParent, std_out);
            break;
        case (int)Menu::updateDB:
            statusReturned = reRun(args, flightDB, errors);
            sendCodeToPipe(childToParent, statusReturned);
            sendMessage(childToParent, errors);
            break;
        case (int)Menu::zipDB:
            flightDB.zipDB();
            sendCodeToPipe(childToParent, EXIT_SUCCESS);
            break;
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

void gracefulExit()
{
    if (kill(pid, SIGUSR1) == 0)
        cout << "SIGUSR1 signal sent to child process." << endl;
    else
        cout << "Failed to send SIGUSR1 signal to child process." << endl;
    int status;
    pid_t terminated_pid = waitpid(pid, &status, WUNTRACED);

    cout << "Child's exit status: " << WEXITSTATUS(status) << endl;
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
