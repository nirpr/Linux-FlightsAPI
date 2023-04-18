#ifndef __UTILITY_H
#define __UTILITY_H

#include "flightDB.h"
#include <ctime>
#include <string>
#include <sstream>

void getDirectoryFromFile(std::string& filePath);
std::string unix_time_to_date(const time_t& unix_time);

#endif