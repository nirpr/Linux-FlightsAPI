#ifndef __FLIGHT_H
#define __FLIGHT_H

#include <string>
// arrivals: callsign , airport-code, takeoff time, arrival time
// full_schedule: callsign, airport-code, time arrive or department
// airplane: airplane-name(icao24),airport-code, takeoff time, arrival time

// ICAO24, callsign, airport-code, takeoff time, arrival time
class Flight {
public:
    Flight(std::string icao24, std::string callsign, std::string origin, std::string destination, int departure_time, int arrival_time)
        : icao24(icao24), callsign(callsign), origin(origin), destination(destination), departure_time(departure_time), arrival_time(arrival_time) {}

    std::string get_icao24() const { return icao24; }
    std::string get_callsign() const { return callsign; }
    std::string get_origin() const { return origin; }
    std::string get_destination() const { return destination; }
    int get_departure_time() const { return departure_time; }
    int get_arrival_time() const { return arrival_time; }
    friend bool operator==(const Flight& lhs , const Flight& rhs);
    friend bool operator==(const Flight& lhs , const std::string& icao24);
    friend bool operator<(const Flight& lhs, const Flight& rhs);
    friend bool operator>(const Flight& lhs, const Flight& rhs);

private:
    const std::string icao24;
    const std::string callsign;
    const std::string origin;
    const std::string destination;
    const int departure_time;
    const int arrival_time;
};

#endif