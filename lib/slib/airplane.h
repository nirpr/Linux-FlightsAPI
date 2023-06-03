#ifndef __AIRPLANE_H
#define __AIRPLANE_H
#include "utility.h"

string airplane(string inputs, const FlightDatabase &DB);
void printFlightsByAirplanesToString(const FlightDatabase &DB, set<string> airplanes, string& std_out);
void printAirplaneFlightDetailsToString(const Flight &flight, string& std_out);
#endif