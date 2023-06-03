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
bool sendTaskToChild(int parentToChild, int OpCode) noexcept(false);
int receiveTaskFromParent(int childToParent) noexcept(false);
string receiveMessage(int pipeRead) noexcept(false);
void sendMessage(int pipeWrite, list<string> strings) noexcept(false);
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
            waitpid(pid, NULL, 0);
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
        cout << "CHILD: before flightDB create." << endl;
        FlightDatabase flightDB(true);
        cout << "CHILD: flightDB created." << endl;
        int opCode = -1;
        while (Running)
        {
            cout << "CHILD: Starting while loop" << endl; // Debugging
            opCode = receiveTaskFromParent(parentToChild);
            cout << "CHILD: opCode Received (" << opCode << ")" << endl; // Debugging
            if (opCode == (int)Menu::Exit || (opCode <= (int)Menu::optionStartRange && (int)Menu::optionEndRange <= opCode))
            {
                // TODO: graceful exit here
                if (opCode == (int)Menu::Exit)
                    Running = false;
                else
                    return EXIT_FAILURE;
            }
            else
            {
                cout << "CHILD: Before task handler mark" << endl; // Debugging
                taskHandler(opCode, parentToChild, childToParent, flightDB);
            }
        }
    }
    catch (const exception &e)
    {
        cout << e.what() << endl; // TODO: Delete later (child can't print | maybe pass message to parent)
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int UserInterface(pid_t &pid, int parentToChild, int childToParent) noexcept(false)
{
    cout << endl
         << "This is parent section [Process id: " << getpid() << "] , [child's id: " << pid << "]." << endl;
    bool Running = true;
    int userChoice, status = 0;
    while (Running)
    {
        sleep(1); // for tests remove at the end.
        printOptions();
        userChoice = getUserDecision((int)Menu::optionStartRange,
                                     (int)Menu::optionEndRange,
                                     (int)ProgramSettings::MaxInputAttempts);
        OptionsHandler(userChoice, parentToChild, childToParent, pid);
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
            cout << "Received SIGINT signal" << endl;
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

int OptionsHandler(int OpCode, int parentToChild, int childToParent, pid_t &pid) noexcept(false) // TODO: Finish Handling logics for parents
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
            cout << "Parent: After sending Child task." << endl;
            string input = getInputFromUser(); // TODO: Finish this
            cout << "Parent: After get input from User." << endl;
            sendMessage(parentToChild, input);
            cout << "Parent: After sendMessage of input to Child." << endl;
            std_out = receiveMessage(childToParent);
            cout << "Parent: After receiveMessage from Child." << endl
                 << endl
                 << endl;
            cout << "Size of std_out " << std_out.size() << endl
                 << " Text:" << endl;
            break;
        }
        case (int)Menu::zipDB:
        {
            cout << "Send task to zip flightDB folder to Child Process" << endl;
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
            gracefulExit(pid);
            break;
        }
    }
    return EXIT_SUCCESS;
}

void taskHandler(int opCode, int parentToChild, int childToParent, FlightDatabase &flightDB) noexcept(false)
{
    switch (opCode)
    {
        case (int)Menu::arrivingFlightsAirport:
        {
            cout << "CHILD: before receiveMessage for arguemnts." << endl;
            string args = receiveMessage(parentToChild);
            cout << "CHILD: after receiveMessage for arguemnts." << endl;
            string std_out = arrivals(args, flightDB);
            cout << "CHILD: after arrivals logic, std_out size is " << std_out.size() << endl;
            sendMessage(childToParent, std_out);
            break;
        }
        case (int)Menu::fullScheduleAirport:
            break;
        case (int)Menu::fullScheduleAircraft:
            break;
        case (int)Menu::updateDB:
            break;
        case (int)Menu::zipDB:
        {
            cout << "taskHandler - zipDB" << endl;
            flightDB.zipDB();
            break;
        }
        default:
            break;
    }
}

bool sendTaskToChild(int parentToChild, int OpCode) noexcept(false)
{
    ssize_t bytesWritten = write(parentToChild, &OpCode, sizeof(OpCode));
    if (bytesWritten == -1)
        throw runtime_error("Failed to write to child.");
    return true;
}

int receiveTaskFromParent(int pipeRead) noexcept(false)
{
    int OpCode = -1;                                               // if -1 returned will be gracefull exit for undefined OpCode
    cout << "CHILD: Waiting for receive task from Parent" << endl; // Debugging
    size_t bytesRead = read(pipeRead, &OpCode, sizeof(OpCode));
    cout << "CHILD: Read response is " << bytesRead << " and  OpCode is " << OpCode << endl; // Debugging
    if (bytesRead == -1)
        throw runtime_error("Failed to read task from pipe.");
    return OpCode;
}

string receiveMessage(int pipeRead) noexcept(false)
{
    char buffer[PIPE_BUF];
    int BytesToRead;
    int bytesRead = 0;
    cout << "Reading - Waiting for message size" << endl;
    bytesRead = read(pipeRead, &BytesToRead, sizeof(BytesToRead));
    if (bytesRead == -1)
        throw runtime_error("Read message from pipe filed!");

    cout << "Reading - need to read " << BytesToRead << ", bytes size readed of the size is " << bytesRead << endl;
    // Read the strings from the child process
    string stringData;
    char chunk[PIPE_BUF];
    while (BytesToRead > 0)
    {
        int chunkSize = min(PIPE_BUF - 1, BytesToRead);
        int bytesRead = read(pipeRead, chunk, chunkSize);
        cout << "Reading - size of chunk is " << bytesRead << "/" << chunkSize << endl;
        if (bytesRead > 0)
        {
            chunk[chunkSize] = '\0';
            stringData.append(chunk); // append string
            BytesToRead -= bytesRead;
            cout << "Reading - Bytes to Read for next read is " << BytesToRead << endl;
        }
        else if (bytesRead == -1)
            throw runtime_error("Read message from pipe filed!");
    }
    cout << "Reading - Final reached" << endl;
    return stringData;
}

void sendMessage(int pipeWrite, list<string> strings) noexcept(false)
{
    // Send the number of reads and sizes to the parent process
    int sizesData;
    for (const string &str : strings)
        sizesData += str.size();
    cout << "Sending - size: " << sizesData << endl;
    if (write(pipeWrite, &sizesData, sizeof(sizesData)) == -1)
        throw runtime_error("Failed to write Message into pipe!");

    // Send the strings to the pipe
    for (const auto &str : strings)
    {
        int remainingSize = str.size();
        int offset = 0;
        // Split the string into smaller chunks if necessary
        while (remainingSize > 0)
        {
            int chunkSize = min(remainingSize, PIPE_BUF);
            if (write(pipeWrite, str.c_str() + offset, chunkSize) == -1)
                throw runtime_error("Failed to write Message into pipe!");
            remainingSize -= chunkSize;
            offset += chunkSize;
            cout << "size left after write is " << remainingSize << endl;
        }
    }
}

void sendMessage(int pipeWrite, string message)
{
    // Send the number of reads and sizes to the parent process
    int sizesData = message.size();
    cout << "Sending - message size: " << sizesData << endl;
    if (write(pipeWrite, &sizesData, sizeof(sizesData)) == -1)
        throw runtime_error("Failed to write Message into pipe!");

    // Send the strings to the pipe

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
        cout << "Sending - size left after write is " << remainingSize << endl;
    }
}

// void sendMessage(int pipeWrite, string message)
// {
//     list<string> messageList;
//     messageList.push_front(message);
//     sendMessage(pipeWrite, messageList);
// }

void gracefulExit(pid_t childPid)
{
    if (kill(childPid, SIGUSR1) == 0)
        cout << "SIGUSR1 signal sent to child process." << endl;
    else
        cout << "Failed to send SIGUSR1 signal to child process." << endl;
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
