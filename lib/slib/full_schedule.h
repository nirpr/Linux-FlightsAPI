#ifndef __FULL_SCHEDULE_H
#define __FULL_SCHEDULE_H
#include "flightDB.h"
#include <iostream>
#include <set>

std::string full_schedule(std::string inputs, const FlightDatabase &DB);
void printFlightsByOrderFromAirportToString(const std::list<Airport> &airportsToPrint, std::string &std_out);
void printArrivingFlightDetailsFSToString(const Flight &flight, std::string &std_out);
void printDepartingFlightDetailsFSToString(const Flight &flight, std::string &std_out);

#endif
