#ifndef __AIRPLANE_H
#define __AIRPLANE_H
#include "flightDB.h"
#include <set>
#include <string>

std::string airplane(std::string inputs, const FlightDatabase &DB);
void printFlightsByAirplanesToString(const FlightDatabase &DB, std::set<std::string> airplanes, std::string &std_out, std::string &std_Error);
void printAirplaneFlightDetailsToString(const Flight &flight, std::string &std_out);

#endif