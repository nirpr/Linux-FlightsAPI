#ifndef __RERUN_H
#define __RERUN_H
#include "flightDB.h"
#include <filesystem>
#include <string>
int reRun(const std::string &inputs, FlightDatabase &DB, std::string &errorStrOut);

#endif