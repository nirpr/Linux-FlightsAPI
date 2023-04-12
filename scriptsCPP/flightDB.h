#ifndef __FLIGHT_DB_H
#define __FLIGHT_DB_H

#include <string>
#include <iostream>
#include "airport.h"
#include "flight.h"

class FlightDatabase {
public:
    void load_db(std::string filename);
    void get_flights_by_airport_name(std::string airport_name) const;
    void get_flights_by_callsign(std::string callsign) const;
    void rerun_script() const;

private:
    void print_flights(std::vector<Flight> flights) const;
    std::vector<Airport> airports;
};

#endif