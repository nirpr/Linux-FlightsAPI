#include "flightsUtility.h"
using namespace std;

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
            else if (returnStatus == EXIT_FAILURE)
            {
                string error = receiveMessage(readFD);
                cerr << error << endl;
                cout << "Database zip failed" << endl;
            }
            return returnStatus;
        }
        case (int)Menu::Exit:
        {
            sendCodeToPipe(writeFD, OpCode);
            returnStatus = receiveCodeFromPipe(readFD);
            if (returnStatus == EXIT_SUCCESS)
                cout << "dbService exited successfully" << endl;
            else if (returnStatus == EXIT_FAILURE)
            {
                string error = receiveMessage(readFD);
                cerr << error << endl;
                cout << "dbService exited with failure" << endl;
            }
            return EXIT_SUCCESS;
            cout << "your'e not suppose to read this" << endl;
        }
    }
    return EXIT_SUCCESS;
}
