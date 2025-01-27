#ifndef __UTILITY_H
#define __UTILITY_H
#define _CRT_SECURE_NO_WARNINGS

#include <cerrno>
#include <csignal>
#include <ctime>
#include <iostream>
#include <limits.h> // used for get PIPE_BUFF
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h> // Pipe libarary

// Enum Declerations
enum class ProgramSettings
{
    MaxInputAttempts = 3
};

enum class Menu
{
    optionStartRange,
    updateDB,
    arrivingFlightsAirport,
    fullScheduleAirport,
    fullScheduleAircraft,
    zipDB,
    Exit,
    optionEndRange
};


void pipeCleanUp(int pipe1, int pipe2);
int sendCodeToPipe(int writePipe, int OpCode) noexcept(false);
int receiveCodeFromPipe(int childToParent) noexcept(false);
std::string receiveMessage(int pipeRead, int* errorInd) noexcept(false);
void sendMessage(int pipeWrite, std::string message, int* errorInd) noexcept(false);

#endif