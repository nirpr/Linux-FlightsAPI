#include "slib/utility.h"
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

void printOptions();
int getUserDecision(int startRange, int endRange, int maxTimes);
void signaHandler(int signal_number);
int LogicProcess(pid_t &cid);
int UserInterface(pid_t &cid);

enum class ProgramSettings
{
    optionStartRange = 1,
    optionEndRange = 7,
    MaxInputAttempts = 3
};

enum class Menu
{
    _,
    incomingFlightsAirport,
    fullScheduleAirport,
    fullScheduleAircraft,
    updateDB,
    zipDB,
    childPID,
    Exit
};

int main(int argc, char **argv)
{
    // TODO: Create Pipeline
    pid_t cid = -1;
    cout << "Program is starting" << endl;

    // Fork the process to Parent and Son
    cid = fork();
    if (cid > 0) // Parent
        UserInterface(cid);
    else if (cid == 0) // Son
        LogicProcess(cid);
    else // TODO: Fork failed handle
    {
        cerr << "fork creation failed" << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int LogicProcess(pid_t &cid) // TODO: Need to handle Functional Logics and pipelines and signals
{
    cout << "Fork created [Process id: " << getpid() << ", [parent process id: " << getppid() << "]." << endl;
    // Waiting for parent pipeline with userchoice input
}

int UserInterface(pid_t &cid) // TODO: Need to handle UI Logic and pipelines and signals
{
    cout << "This is parent section [Process id: " << getpid() << "] , [child's id: " << cid << " ]." << endl;
    bool Alive = true;
    int userChoice;
    while (Alive)
    {
        printOptions();
        userChoice = getUserDecision((int)ProgramSettings::optionStartRange,
                                     (int)ProgramSettings::optionEndRange,
                                     (int)ProgramSettings::MaxInputAttempts);
        // TODO: Handle userChoice and tranfer it to the Son process with pipeline

        // TODO: Wait for response from the son process and get from pipeline the results
    }
}

void signaHandler(int signal_number) // TODO: need to handle signals
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
            // Handle segmentation fault gracefully
            cout << "Segmentation fault occurred. Exiting gracefully" << endl;
            exit(1);
            break;
        case SIGUSR1:
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
             << "Your choice is invalid, please try again: ";
        cin >> UserDecision;
    }
    if (timesFailed == maxTimes)
    {
        cout << timesFailed << " Times entered invalid choice, Exit from the program." << endl;
        return (int)Menu::Exit;
    }
    return UserDecision;
}
