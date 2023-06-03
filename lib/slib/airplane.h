#ifndef __AIRPLANE_H
#define __AIRPLANE_H
#include "utility.h"

void printAirplaneFlightDetails(const Flight &flight);
void printFlightsByAirplanes(const FlightDatabase &DB, char **argv, int airplanes_amount);
#endif