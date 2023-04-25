#ifndef __FULL_SCHEDULE_H
#define __FULL_SCHEDULE_H

#include <iostream>
#include <set>
#include "slib/utility.h"

void printArrivingFlightDetailsFS(const Flight& flight);
void printDepartingFlightDetailsFS(const Flight& flight);
void printFlightsByOrderFromAirport(const std::list<Airport>& airportsToPrint);

#endif
