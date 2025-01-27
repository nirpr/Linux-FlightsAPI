#ifndef __ARRIVALS_H
#define __ARRIVALS_H

#include "flightDB.h"
#include <iostream>
#include <set>
#include <sstream>

std::string arrivals(std::string inputs, const FlightDatabase &DB);
void printArrivingFlightDetailsToString(const Flight &flight, std::string &std_out);
void printFlightsArrivalFromAirportToString(const std::list<Airport> &airportsToPrint, std::string &std_out);

#endif