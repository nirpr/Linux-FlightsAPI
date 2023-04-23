#ifndef __UTILITY_H
#define __UTILITY_H
#define _CRT_SECURE_NO_WARNINGS
#include "flightDB.h"
#include <ctime>
#include <string>
#include <sstream>
#include <time.h>

void getDirectoryFromFile(std::string& filePath);
std::string unix_time_to_date(const time_t& unix_time);
void printArrivingFlightDetails(const Flight& flight);
void printArrivingFlightDetailsFS(const Flight& flight);
void printDepartingFlightDetailsFS(const Flight& flight);
void printFlightsByOrderFromAirport(const std::list<Airport>& airportsToPrint);
void printFlightsArrivalFromAirport(const std::list<Airport>& airportsToPrint);

#endif