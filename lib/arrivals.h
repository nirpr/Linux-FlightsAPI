#ifndef __ARRIVALS_H
#define __ARRIVALS_H

#include <iostream>
#include <set>
#include "slib/utility.h"
#include "slib/flightDB.h"

void printArrivingFlightDetails(const Flight& flight);
void printFlightsArrivalFromAirport(const std::list<Airport>& airportsToPrint);

#endif