#ifndef __FLIGHTSUTILITY_H
#define __FLIGHTSUTILITY_H
#define _CRT_SECURE_NO_WARNINGS

#include "../slib/utility.h"
#include <limits>

// Function Declerations
void printOptions();
int getUserDecision(int startRange, int endRange, int maxTimes); 
std::string getInputFromUser(); 
int OptionsHandler(int OpCode, int parentToChild, int childToParent) noexcept(false);
int UserInterface(int parentToChild, int childToParent) noexcept(false);

#endif