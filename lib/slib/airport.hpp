#ifndef __AIRPORT_HPP
#define __AIRPORT_HPP

#include <list>
#include "flight.hpp"

class Airport {
public:
    enum class Directions { arriving, destinations, both };
    Airport(const std::string& name) : name(name.c_str()), num_flights(0), num_flightsArv(0),num_flightsDpt(0), ariportDataLoaded(false) {}
    void add_flightDpt(const Flight& flight);
    void add_flightArv(const Flight& flight);
    const std::string& get_name() const { return name; }
    int get_num_flights() const { return num_flights; }
    int get_num_flightsDpt() const { return num_flightsDpt; }
    int get_num_flightsArv() const { return num_flightsArv; }
    const std::list<Flight>& get_flightsDpt() const { return flightsDpt; }
    const std::list<Flight>& get_flightsArv() const { return flightsArv; }
    friend bool operator==(const Airport& lhs, const Airport& rhs);
    bool changeDataLoadStatus(bool newStatus);
private:
    std::string name;  // airport-code
    int num_flights;
    int num_flightsDpt;
    int num_flightsArv;
    bool ariportDataLoaded;
    std::list<Flight> flightsDpt;
    std::list<Flight> flightsArv;
};

#endif