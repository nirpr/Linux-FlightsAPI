#ifndef __UTILITY_H
#define __UTILITY_H
#define _CRT_SECURE_NO_WARNINGS

#include "airplane.h"
#include "arrivals.h"
#include "flightDB.h"
#include "full_schedule.h"
#include "reRun.h"

#include <cerrno>
#include <csignal>
#include <ctime>
#include <iostream>
#include <limits.h> // used for get PIPE_BUFF
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h> // Pipe libarary

// Defines
#define READ_END 0
#define WRITE_END 1
extern pid_t pid;

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

// Function Declerations
void getDirectoryFromFile(std::string &filePath);
std::string unix_time_to_date(const time_t &unix_time);
void printOptions();
void pipeCleanUp(int parentToChild[2], int childToParent[2]);
int getUserDecision(int startRange, int endRange, int maxTimes);
std::string getInputFromUser();
void signalHandlerParent(int signal_number);
void signalHandlerChild(int signal_number);
int LogicProcess(int parentToChild, int childToParent) noexcept(false);
int OptionsHandler(int OpCode, int parentToChild, int childToParent) noexcept(false);
void taskHandler(int opCode, int parentToChild, int childToParent, FlightDatabase &flightDB) noexcept(false);
int UserInterface(int parentToChild, int childToParent) noexcept(false);
bool sendCodeToPipe(int writePipe, int OpCode) noexcept(false);
int receiveCodeFromPipe(int childToParent) noexcept(false);
std::string receiveMessage(int pipeRead) noexcept(false);
void sendMessage(int pipeWrite, std::string message) noexcept(false);
void gracefulExit();

#endif