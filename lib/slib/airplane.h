#ifndef __AIRPLANE_H
#define __AIRPLANE_H
#include "utility.h"

std::string airplane(std::string inputs, const FlightDatabase &DB);
void printFlightsByAirplanesToString(const FlightDatabase &DB, std::set<std::string> airplanes, std::string &std_out);
void printAirplaneFlightDetailsToString(const Flight &flight, std::string &std_out);
#endif