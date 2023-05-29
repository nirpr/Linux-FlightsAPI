// import libarary
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
#define CHUNK_SIZE PIPE_BUF

// Function Declerations
void printOptions();
void pipeCleanUp(int parentToChild[2], int childToParent[2]);
int getUserDecision(int startRange, int endRange, int maxTimes);
void signalHandlerParent(int signal_number);
void signalHandlerChild(int signal_number);
int LogicProcess(pid_t &pid, int parentToChild[2], int childToParent[2]);
int OptionsHandler(int OpCode, int parentToChild[2], int childToParent[2], pid_t &pid);
int UserInterface(pid_t &pid, int parentToChild[2], int childToParent[2]);
bool sendTaskToChild(int parentToChild, int OpCode);
int receiveTaskFromParent(int childToParent);
string receiveResults(int parentToChild);

// Enum Declerations
enum class ProgramSettings
{
    optionStartRange = 1,
    optionEndRange = 7,
    MaxInputAttempts = 3
};

enum class Menu
{
    _,
    arrivingFlightsAirport,
    fullScheduleAirport,
    fullScheduleAircraft,
    updateDB,
    zipDB,
    childPID,
    Exit
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
    pid_t pid = -1;
    int returnedStatus;
    pid = fork();
    if (pid > 0) // Parent
    {
        // Parent Pipe Setup Continue
        close(parentToChild[READ_END]);  // Parent dont read from this pipe
        close(childToParent[WRITE_END]); // Parent dont write to this pipe
        // Parent Signals Setup
        signal(SIGINT, signalHandlerParent);                               // Register SIGINT of parent
        signal(SIGUSR1, signalHandlerParent);                              // Register SIGUSR1 of parent
        returnedStatus = UserInterface(pid, parentToChild, childToParent); // Handle UI
        pipeCleanUp(parentToChild, childToParent);                         // Close the pipes between Parent and Child}
        return returnedStatus;
    }
    else if (pid == 0) // Child
    {
        // Child Pipe Setup Continue
        close(parentToChild[WRITE_END]); // Child dont write to this pipe
        close(childToParent[READ_END]);  // Child dont read from this pipe
        // Child Signals Setup
        signal(SIGINT, signalHandlerChild);                               // Register SIGINT of Child
        signal(SIGUSR1, signalHandlerChild);                              // Register SIGUSR1 of Child
        returnedStatus = LogicProcess(pid, parentToChild, childToParent); // Handle Task and logics from Parent
        pipeCleanUp(parentToChild, childToParent);                        // Close the pipes between Parent and Child}
        return returnedStatus;
    }
    else // TODO: Fork failed handle
    {
        cerr << "Failed to create child process." << endl;
        return EXIT_FAILURE;
    }
}

int LogicProcess(pid_t &pid, int parentToChild[2], int childToParent[2]) // TODO: Need to handle Functional Logics and pipelines and signals
{
    cout << "Fork created [Process id: " << getpid() << ", [parent process id: " << getppid() << "]." << endl;
    signal(SIGINT, signalHandlerChild);
}

int UserInterface(pid_t &pid, int parentToChild[2], int childToParent[2]) // TODO: Need to handle UI Logic and pipelines and signals
{
    cout << "This is parent section [Process id: " << getpid() << "] , [child's id: " << pid << " ]." << endl;
    bool Alive = true;
    int userChoice, status = 0;
    while (Alive)
    {
        printOptions();
        userChoice = getUserDecision((int)ProgramSettings::optionStartRange,
                                     (int)ProgramSettings::optionEndRange,
                                     (int)ProgramSettings::MaxInputAttempts);
        // TODO: Handle userChoice and tranfer it to the Child process with pipeline in OptionsHandler
        OptionsHandler(userChoice, parentToChild, childToParent, pid);
        // TODO: Wait for response from the Child process and get from pipeline the results
    }
}

void pipeCleanUp(int parentToChild[2], int childToParent[2])
{
    // Close Parent Pipes
    close(parentToChild[READ_END]);
    close(parentToChild[WRITE_END]);

    // Close Child Pipes
    close(childToParent[READ_END]);
    close(childToParent[WRITE_END]);
}

void signalHandlerParent(int signal_number) // TODO: need to handle signals
{
    switch (signal_number)
    {
        case SIGINT:
            // TODO: Handle SIGINT fault will zip file and gracefully exit
            cout << "Received SIGINT signal" << endl;
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

void signalHandlerChild(int signal_number) // TODO: need to handle signals
{
    switch (signal_number)
    {
        case SIGINT:
            cout << "Received SIGINT signal" << endl;
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
    while ((UserDecision < startRange || endRange < UserDecision) && timesFailed < maxTimes)
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

int OptionsHandler(int OpCode, int parentToChild[2], int childToParent[2], pid_t &pid) // TODO: Finish Handling logics for parents
{
    string results;
    switch (OpCode)
    {
        // Same Functionality for 1-3 (Maybe 4?) - TODO: Check for 4.
        case (int)Menu::arrivingFlightsAirport: // Same Logic (NO Break)
        case (int)Menu::fullScheduleAirport:    // Same Logic (NO Break)
        case (int)Menu::fullScheduleAircraft:   // Same Logic (Handle and break for 1-3 OpCodes)
            sendTaskToChild(parentToChild[WRITE_END], OpCode);
            results = receiveResults(childToParent[READ_END]);
            cout << results;
            break;
        case (int)Menu::updateDB:
            // TODO: Call reRun program (function not program). - (Child Handling)
            break;
        case (int)Menu::zipDB:
            // TODO Create function with libzip for zip the DB and call this function (Child Handling)
            break;
        case (int)Menu::childPID:
            cout << "The Process Child's ID: " << pid << endl;
            break;
        case (int)Menu::Exit:
            // TODO: Graceful Exit
            break;
    }
}

bool sendTaskToChild(int parentToChild, int OpCode)
{
    int status;
    ssize_t bytesWritten = write(parentToChild, &OpCode, sizeof(OpCode));
    if (bytesWritten == -1)
    {
        cerr << "Failed to write to child." << endl;
        return false; // TODO: Error Handling instend
    }
    return true;
}

int receiveTaskFromParent(int pipeRead)
{
    int OpCode;
    ssize_t bytesRead = read(pipeRead, &OpCode, sizeof(OpCode));
    if (bytesRead == -1)
    {
        cerr << "Failed to receive result from Parent." << endl;
        // TODO: Graceful exit
    }
    close(pipeRead);
    return OpCode;
}

string receiveResults(int pipeRead)
{
    string results;
    char buffer[CHUNK_SIZE];
    while (true)
    {
        ssize_t bytesRead = read(pipeRead, buffer, CHUNK_SIZE);
        if (bytesRead == 0)
            break;
        if (bytesRead == -1)
        {
            std::cerr << "Failed to receive result." << std::endl;
            // TODO: Graceful exit
        }
        results += buffer; // Append the string and continue to read till fd is empty
    }
    return results;
}