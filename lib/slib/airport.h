#ifndef __AIRPORT_H
#define __AIRPORT_H

#include <string>
#include <iostream>
#include <list>
#include "flight.h"

class Airport {
public:
    Airport(std::string name) : name(name), num_flights(0), num_flightsArv(0),num_flightsDpt(0) {}
    void add_flightDpt(const Flight& flight);
    void add_flightArv(const Flight& flight);
    std::string get_name() const { return name; }
    int get_num_flights() const { return num_flights; }
    int get_num_flightsDpt() const { return num_flightsDpt; }
    int get_num_flightsArv() const { return num_flightsArv; }
    const std::list<Flight>& get_flightsDpt() const { return flightsDpt; }
    const std::list<Flight>& get_flightsArv() const { return flightsArv; }
    friend bool operator==(const Airport& lhs, const Airport& rhs);

private:
    std::string name;  // airport-code
    int num_flights;
    int num_flightsDpt;
    int num_flightsArv;
    std::list<Flight> flightsDpt;
    std::list<Flight> flightsArv;
};

void Airport::add_flightDpt(const Flight& flight) {
    flightsDpt.push_back(std::move(flight));
    this->num_flights++;
    this->num_flightsDpt++;
}
void Airport::add_flightArv(const Flight& flight) {
    flightsArv.push_back(std::move(flight));
    this->num_flights++;
    this->num_flightsArv++;
}

#endif