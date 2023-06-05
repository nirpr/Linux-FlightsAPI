// import libarary
#include "slib/airplane.h"
#include "slib/arrivals.h"
#include "slib/full_schedule.h"
#include "slib/reRun.h"
#include "slib/utility.h"

#include <cerrno>
#include <csignal>
#include <iostream>
#include <limits.h> // used for get PIPE_BUFF
#include <stdio.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h> // Pipe libarary
using namespace std;

// defines
#define READ_END 0
#define WRITE_END 1

pid_t pid = -1; // global so we can access the child's pid whenever we need

// Function Declerations
void printOptions();
void pipeCleanUp(int parentToChild[2], int childToParent[2]);
int getUserDecision(int startRange, int endRange, int maxTimes);
string getInputFromUser();
void signalHandlerParent(int signal_number);
void signalHandlerChild(int signal_number);
int LogicProcess(pid_t &pid, int parentToChild, int childToParent) noexcept(false);
int OptionsHandler(int OpCode, int parentToChild, int childToParent, pid_t &pid) noexcept(false);
void taskHandler(int opCode, int parentToChild, int childToParent, FlightDatabase &flightDB) noexcept(false);
int UserInterface(pid_t &pid, int parentToChild, int childToParent) noexcept(false);
bool sendCodeToPipe(int writePipe, int OpCode) noexcept(false);
int receiveCodeFromPipe(int childToParent) noexcept(false);
string receiveMessage(int pipeRead) noexcept(false);
void sendMessage(int pipeWrite, string message) noexcept(false);
void gracefulExit(pid_t childPid);

// Enum Declerations
enum class ProgramSettings
{
    MaxInputAttempts = 3
};

enum class Menu
{
    optionStartRange,
    arrivingFlightsAirport,
    fullScheduleAirport,
    fullScheduleAircraft,
    updateDB,
    zipDB,
    childPID,
    Exit,
    optionEndRange
};

// Functions
int main(int argc, char **argv)
{
    cout << "Program is starting" << endl;
    int parentToChild[2] = {0, 0}, childToParent[2] = {0, 0}; // create pipes for communication
    // ---- First Pipe setup ----
    if (pipe(parentToChild) == -1 || pipe(childToParent) == -1)
    {
        cerr << "Failed to create pipe." << endl;
        return EXIT_FAILURE;
    }
    // ---- Pipe setup End ----

    // Fork the process to Parent and Child
    int returnedStatus;
    try
    {
        pid = fork();
        if (pid > 0) // Parent
        {
            // Parent Pipe Setup Continue
            close(parentToChild[READ_END]);  // Parent dont read from this pipe
            close(childToParent[WRITE_END]); // Parent dont write to this pipe

            // Parent Signals Setup
            signal(SIGINT, signalHandlerParent); // Register SIGINT of parent

            // UI - Parent Handling
            returnedStatus = UserInterface(pid, parentToChild[WRITE_END], childToParent[READ_END]); // Handle UI
            pipeCleanUp(parentToChild, childToParent);                                              // Close the pipes between Parent and Child}
            return returnedStatus;
        }
        else if (pid == 0) // Child
        {
            // Child Pipe Setup Continue
            close(parentToChild[WRITE_END]); // Child dont write to this pipe
            close(childToParent[READ_END]);  // Child dont read from this pipe
            // Child Signals Setup
            signal(SIGINT, signalHandlerChild);  // register SIGINT to ignore and handle from the parent
            signal(SIGUSR1, signalHandlerChild); // Register SIGUSR1 of Child

            // Logic Process - Child Handling
            returnedStatus = LogicProcess(pid, parentToChild[READ_END], childToParent[WRITE_END]); // Handle Task and logics from Parent
            pipeCleanUp(parentToChild, childToParent);                                             // Close the pipes between Parent and Child}
            return returnedStatus;
        }
        else // Fork failed handle
        {
            cerr << "Failed to create child process." << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const exception &e) // Exit for critical Error that didn't have any specific handle case
    {
        cerr << e.what() << endl;
        if (pid > 0) // Parent
        {
            gracefulExit(pid);
            // TODO: Maybe add check if child is alive and make graceful exit for child before exit.
        }
        else if (pid == 0)
        {
            // TODO: Graceful exit
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
}

int LogicProcess(pid_t &pid, int parentToChild, int childToParent) noexcept(false)
{
    bool Running = true;
    try
    {
        FlightDatabase flightDB(true);
        // Opening Message for Parent
        sendMessage(childToParent, "Child Process: Ready !\n");
        int opCode = -1;
        while (Running)
        {
            cout << "Child: Start loop" << endl;
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
            {
                taskHandler(opCode, parentToChild, childToParent, flightDB);
            }
        }
    }
    catch (const exception &e)
    {
        int errorCode = errno;
        if (!(errorCode == EBADF || errorCode == EIO || errorCode == ENOSPC || errorCode == EPIPE)) // Check if failed isn't in pipes
            sendMessage(childToParent, e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int UserInterface(pid_t &pid, int parentToChild, int childToParent) noexcept(false)
{
    cout << endl
         << "This is parent section [Process id: " << getpid() << "] , [child's id: " << pid << "]." << endl;
    bool Running = true;
    int userChoice, returnedStatus = EXIT_FAILURE; // returnedStatus default fail till Option Handler return something
    int child_status;
    pid_t result;
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
            sleep(1);
            printOptions();
            userChoice = getUserDecision((int)Menu::optionStartRange,
                                         (int)Menu::optionEndRange,
                                         (int)ProgramSettings::MaxInputAttempts);
            returnedStatus = OptionsHandler(userChoice, parentToChild, childToParent, pid);
            if (userChoice == (int)Menu::Exit)
                Running = false;
        }
        else
        {
            if (WIFEXITED(status)) // child process exited normaly
            {
                int exitStatus = WEXITSTATUS(status);
                cout << "child process exited with status: " << exitStatus << endl;
            }
            else if (WIFSIGNALED(status)) // child process terminated
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

void signalHandlerParent(int signal_number) // TODO: need to handle signals
{
    switch (signal_number)
    {
        case SIGINT:
            cout << endl
                 << "Received SIGINT signal" << endl;
            gracefulExit(pid); // this is the child's pid
            exit(SIGINT);
            break;
        case SIGTERM:
            cout << "Received SIGTERM signal" << endl;
            break;
        case SIGSEGV:
            cout << "Received SIGSEGV signal" << endl;
            exit(1);
            break;
        case SIGUSR1:
            // TODO: Handle SIGUSR1 fault gracefully exit
            cout << "Received SIGUSR1 signal" << endl;
            break;

        default:
            cout << "Received unknown signal" << endl;
    }
}

void signalHandlerChild(int signal_number)
{
    switch (signal_number)
    {
        case SIGINT:
            cout << "recived SIGINT, ignoring" << endl;
            break;
        case SIGTERM:
            cout << "Received SIGTERM signal" << endl;
            break;
        case SIGSEGV:
            cout << "Received SIGSEGV signal" << endl;
            cerr << "Segmentation fault occurred. Exiting gracefully" << endl;
            FlightDatabase::zipDB();
            exit(SIGSEGV);
            break;
        case SIGUSR1:
            cout << endl
                 << "zipping and terminating..." << endl;
            FlightDatabase::zipDB();
            exit(SIGUSR1);
            break;

        default:
            cout << "Received unknown signal" << endl;
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
    int UserDecision, timesFailed = 0;
    cin >> UserDecision;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
    while ((UserDecision <= startRange || endRange <= UserDecision) && timesFailed < maxTimes)
    {
        timesFailed++;
        if (timesFailed == maxTimes - 1)
        {
            cout << "Last time chance before exit from the progam." << endl;
        }
        cout << "Fail "
             << timesFailed << "/"
             << maxTimes
             << "Your choice is invalid, please try again <" << startRange - 1 << "-" << endRange - 1 << "> : ";
        cin >> UserDecision;
    }
    if (timesFailed == maxTimes)
    {
        cout << timesFailed << " Times entered invalid choice, Exit from the program." << endl;
        return (int)Menu::Exit;
    }
    return UserDecision;
}

int OptionsHandler(int OpCode, int parentToChild, int childToParent, pid_t &pid) noexcept(false) // TODO: Finish Handling logics for parents
{
    string std_out;
    int returnStatus;
    switch (OpCode)
    {
        // Same Functionality for 1-4
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
        case (int)Menu::updateDB: // Same Logic (Handle and break for 1-3 OpCodes)
        {
            sendCodeToPipe(parentToChild, OpCode);
            string input = getInputFromUser();
            sendMessage(parentToChild, input);
            cout << "Waiting for DB update." << endl;
            returnStatus = receiveCodeFromPipe(childToParent);
            if (returnStatus == EXIT_SUCCESS)
                cout << "DB Updated successfully" << endl;
            else if (returnStatus == EXIT_FAILURE)
                cout << "DB Update failed." << endl;
            return returnStatus;
        }
        case (int)Menu::zipDB:
        {
            cout << "Send task to zip flightDB folder to Child Process" << endl;
            sendCodeToPipe(parentToChild, OpCode);
            returnStatus = receiveCodeFromPipe(childToParent);
            if (returnStatus == EXIT_SUCCESS)
                cout << "DB Zipped successfully" << endl;
            else if (returnStatus == EXIT_FAILURE)
                cout << "DB Zip failed." << endl;
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
            gracefulExit(pid);
            terminated_pid = waitpid(pid, &status, WUNTRACED); // wait for exit status from child process
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
            // TODO: Handle errors send and recieve in parent
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
        throw runtime_error("Failed write to Pipe.");
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
        throw runtime_error("Read message from pipe filed!");

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
            throw runtime_error("Read message from pipe filed!");
    }
    return stringData;
}

void sendMessage(int pipeWrite, string message) noexcept(false)
{
    // Sending the size of the message
    int sizesData = message.size();
    if (write(pipeWrite, &sizesData, sizeof(sizesData)) == -1)
        throw runtime_error("Failed to write Message into pipe!");

    // Send the string to the pipe
    int remainingSize = message.size();
    int offset = 0;
    // Split the string into smaller chunks if necessary
    while (remainingSize > 0)
    {
        int chunkSize = min(remainingSize, PIPE_BUF);
        if (write(pipeWrite, message.c_str() + offset, chunkSize) == -1)
            throw runtime_error("Failed to write Message into pipe!");
        remainingSize -= chunkSize;
        offset += chunkSize;
    }
}

void gracefulExit(pid_t childPid)
{
    if (kill(childPid, SIGUSR1) == 0)
        cout << "SIGUSR1 signal sent to child process." << endl;
    else
        cout << "Failed to send SIGUSR1 signal to child process." << endl;
    int status;
    pid_t terminated_pid = waitpid(childPid, &status, WUNTRACED);

    cout << "child status: " << WEXITSTATUS(status) << endl;
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
