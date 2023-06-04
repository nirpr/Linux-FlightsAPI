#ifndef __RERUN_H
#define __RERUN_H
#include "utility.h"
#include <filesystem>

int reRun(const std::string& inputs, FlightDatabase &DB, std::string& errorStrOut);

#endif