#ifndef __UTILITY_H
#define __UTILITY_H

#include <string>
#include <iostream>
#include <string>
#include <vector>
#include "flight.h"

class Airport {
public:
    Airport(std::string name) : name(name), num_flights(0) {}
    void add_flight(const Flight& flight);
    std::string get_name() const { return name; }
    int get_num_flights() const { return num_flights; }
    const std::vector<Flight>& get_flights() const { return flights; }
private:
    std::string name;  // airport code
    int num_flights;
    std::vector<Flight> flights;
};

void Airport::add_flight(const Flight& flight) {
    flights.push_back(std::move(flight));
    this->num_flights++;
}

#endif