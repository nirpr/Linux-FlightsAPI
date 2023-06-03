// import libarary
#include "slib/airplane.h"
#include "slib/arrivals.h"
#include "slib/full_schedule.h"
#include "slib/utility.h"

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


mutex mtx;
pid_t pid = -1; // global so we can access the child's pid whenever we need

// Function Declerations
void printOptions();
void pipeCleanUp(int parentToChild[2], int childToParent[2]);
int getUserDecision(int startRange, int endRange, int maxTimes);
string getInputFromUser();
void signalHandlerParent(int signal_number);
void signalHandlerChild(int signal_number);
int LogicProcess(pid_t &pid, int parentToChild, int childToParent);
int OptionsHandler(int OpCode, int parentToChild, int childToParent, pid_t &pid);
void taskHandler(int opCode, int parentToChild, int childToParent, FlightDatabase &flightDB);
int UserInterface(pid_t &pid, int parentToChild, int childToParent);
bool sendTaskToChild(int parentToChild, int OpCode);
int receiveTaskFromParent(int childToParent);
string receiveMessage(int pipeRead);
void sendMessage(int pipeWrite, list<string> strings);
void sendMessage(int pipeWrite, string message);
void gracefulExit(pid_t childPid); // need to implement

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
    // ---- First Pipe setup ----
    int parentToChild[2] = {0, 0}, childToParent[2] = {0, 0}; // create pipes for communication
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
    cout << endl
         << "Fork created [Process id: " << getpid() << ", [parent process id: " << getppid() << "]." << endl;
    try
    {
        FlightDatabase flightDB(true);
        int opCode = -1;
        while (Running)
        {
            opCode = receiveTaskFromParent(parentToChild);
            if (opCode == (int)Menu::Exit || (opCode <= (int)Menu::optionStartRange && (int)Menu::optionEndRange <= opCode))
            {
                // TODO: graceful exit here
                if (opCode == (int)Menu::Exit)
                    Running = false;
                else
                    return EXIT_FAILURE;
            }
            else
                taskHandler(opCode, parentToChild, childToParent, flightDB);
        }
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl; // TODO: Delete later (child can't print | maybe pass message to parent)
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int UserInterface(pid_t &pid, int parentToChild, int childToParent) noexcept(false) // TODO: Need to handle UI Logic and pipelines and signals
{
    cout << endl
         << "This is parent section [Process id: " << getpid() << "] , [child's id: " << pid << "]." << endl;
    bool Running = true;
    int userChoice, status = 0;
    while (Running)
    {
        printOptions();
        userChoice = getUserDecision((int)Menu::optionStartRange,
                                     (int)Menu::optionEndRange,
                                     (int)ProgramSettings::MaxInputAttempts);
        // TODO: Handle userChoice and tranfer it to the Child process with pipeline in OptionsHandler
        OptionsHandler(userChoice, parentToChild, childToParent, pid);
        // TODO: Wait for response from the Child process and get from pipeline the results
        if (userChoice == (int)Menu::Exit)
            Running = false;
    }
    return EXIT_SUCCESS;
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
            cout << "Received SIGINT signal" << endl;
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
        switch(signal_number) {
        case SIGINT:
			cout << "Received SIGINT signal" << endl;
            break;
        case SIGTERM:
			cout << "Received SIGTERM signal" << endl;
            break;
		case SIGSEGV:
			cout << "Received SIGSEGV signal" << endl;
			cout << "Segmentation fault occurred. Exiting gracefully" << endl;
            FlightDatabase::zipDB();
			exit(1);
            break;
        case SIGUSR1:
			cout << "zippin and terminating..." << endl;
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
             << "Your choice is invalid, please try again <" << startRange << "-" << endRange << "> : ";
        cin >> UserDecision;
    }
    if (timesFailed == maxTimes)
    {
        cout << timesFailed << " Times entered invalid choice, Exit from the program." << endl;
        return (int)Menu::Exit;
    }
    return UserDecision;
}

int OptionsHandler(int OpCode, int parentToChild, int childToParent, pid_t &pid) // TODO: Finish Handling logics for parents
{
    string std_out;
    switch (OpCode)
    {
        // Same Functionality for 1-4
        case (int)Menu::arrivingFlightsAirport: // Same Logic (NO Break)
        case (int)Menu::fullScheduleAirport:    // Same Logic (NO Break)
        case (int)Menu::fullScheduleAircraft:   // Same Logic (NO Break)
        case (int)Menu::updateDB:               // Same Logic (Handle and break for 1-3 OpCodes)
        {
            sendTaskToChild(parentToChild, OpCode);
            string input = getInputFromUser(); // TODO: Finish this
            sendMessage(parentToChild, input);
            std_out = receiveMessage(childToParent);
            // Add print for std_out if not empty
            break;
        }
        case (int)Menu::zipDB:
        {
            sendTaskToChild(parentToChild, OpCode);
            break;
        }
        case (int)Menu::childPID:
        {
            cout << "The Process Child's ID: " << pid << endl;
            break;
        }
        case (int)Menu::Exit:
        {
            // TODO: Graceful Exit
            break;
        }
    }
    return 0;
}

void taskHandler(int opCode, int parentToChild, int childToParent, FlightDatabase &flightDB)
{
    switch (opCode)
    {
        case (int)Menu::arrivingFlightsAirport:
        {
            string args = receiveMessage(parentToChild);
            string std_out = arrivals(args, flightDB);
            cout << std_out;
            break;
        }
        case (int)Menu::fullScheduleAirport:
            break;
        case (int)Menu::fullScheduleAircraft:
            break;
        case (int)Menu::updateDB:
            break;
        case (int)Menu::zipDB:

            break;
        default:
            break;
    }
}

bool sendTaskToChild(int parentToChild, int OpCode)
{
    ssize_t bytesWritten = write(parentToChild, &OpCode, sizeof(OpCode));
    if (bytesWritten == -1)
    {
        cerr << "Failed to write to child." << endl;
        return false; // TODO: Error Handling instend maybe
    }
    return true;
}

int receiveTaskFromParent(int pipeRead)
{
    int OpCode = -1; // if -1 returned will be gracefull exit for undefined OpCode
    read(pipeRead, &OpCode, sizeof(OpCode));
    return OpCode;
}

string receiveMessage(int pipeRead)
{
    char buffer[PIPE_BUF];
    size_t BytesToRead;
    read(pipeRead, &BytesToRead, sizeof(BytesToRead));

    // Read the strings from the child process
    int bytesRead = 0;
    string stringData;
    char chunk[PIPE_BUF];
    while (BytesToRead > 0)
    {
        size_t chunkSize = min((size_t)PIPE_BUF, BytesToRead);
        size_t bytesRead = read(pipeRead, chunk, chunkSize);
        if (bytesRead > 0)
        {
            stringData.append(chunk); // append string
            BytesToRead -= bytesRead;
        }
        else if (bytesRead == -1)
            throw runtime_error("Read message from pipe filed!");
    }
    return stringData;
}

void sendMessage(int pipeWrite, list<string> strings)
{
    // Send the number of reads and sizes to the parent process
    int numReads = strings.size();
    size_t sizesData;
    for (const string &str : strings)
        sizesData += str.size();
    write(pipeWrite, &sizesData, sizeof(sizesData));

    // Send the strings to the pipe
    for (const auto &str : strings)
    {
        int remainingSize = str.size();
        int offset = 0;
        // Split the string into smaller chunks if necessary
        while (remainingSize > 0)
        {
            int chunkSize = min(remainingSize, PIPE_BUF);
            write(pipeWrite, str.c_str() + offset, chunkSize);
            remainingSize -= chunkSize;
            offset += chunkSize;
        }
    }
}

void sendMessage(int pipeWrite, string message)
{
    list<string> messageList;
    messageList.push_front(message);
    sendMessage(pipeWrite, messageList);
}

void gracefulExit(pid_t childPid)
{
    kill(childPid, SIGUSR1);
}

