#ifndef __FULL_SCHEDULE_H
#define __FULL_SCHEDULE_H

#include "utility.h"
#include <iostream>
#include <set>

void printFlightsByOrderFromAirportToString(const list<Airport> &airportsToPrint,string& std_out);
void printArrivingFlightDetailsFSToString(const Flight &flight, string& std_out);
void printDepartingFlightDetailsFSToString(const Flight &flight, string& std_out);


#endif
