#ifndef __FLIGHT_H
#define __FLIGHT_H

#include <string>
#include <iostream>
#include <string>
#include <vector>

class Flight {
public:
    Flight(std::string callsign, std::string origin, std::string destination, int departure_time, int arrival_time)
        : callsign(callsign), origin(origin), destination(destination), departure_time(departure_time), arrival_time(arrival_time) {}

    std::string get_callsign() const { return callsign; }
    std::string get_origin() const { return origin; }
    std::string get_destination() const { return destination; }
    int get_departure_time() const { return departure_time; }
    int get_arrival_time() const { return arrival_time; }

private:
    const std::string callsign;
    const std::string origin;
    const std::string destination;
    const int departure_time;
    const int arrival_time;
};

#endif