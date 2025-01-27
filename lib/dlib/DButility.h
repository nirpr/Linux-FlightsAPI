#ifndef __DBUTILITY_H
#define __DBUTILITY_H
#define _CRT_SECURE_NO_WARNINGS

#include "airplane.h"
#include "arrivals.h"
#include "flightDB.h"
#include "full_schedule.h"
#include "reRun.h"
#include "../slib/utility.h"

// Function Declerations
void getDirectoryFromFile(std::string &filePath);
std::string unix_time_to_date(const time_t &unix_time);
int LogicProcess(int parentToChild, int childToParent) noexcept(false);
void taskHandler(int opCode, int parentToChild, int childToParent, FlightDatabase &flightDB) noexcept(false);







#endif