#ifndef __FLIGHT_DB_H
#define __FLIGHT_DB_H

#include <set>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <filesystem>

#include "airport.h"
#include "flight.h"

class FlightDatabase {
public:
    enum class DataParameters { icao24, callsign, origin, destination, departure_time, arrival_time, totalParm };
    enum class Directions { arriving, destinations, both };
    FlightDatabase() : loaded(false) {}
    bool load_db(const std::string& dataBasePath, const std::string& airportCode);
    const std::list<Airport> get_flights_by_airport_name(const std::set<std::string>& airport_name, int ToFromBoth) const;
    std::set<std::string> get_airports_names() const;
    const std::set<Flight> getAirplanes(const std::string& icao24) const;
    const std::set<Flight> getAirplanes(const std::set<std::string>& icao24_required) const;
private:
    void getFlightsFromFiles(std::ifstream& inFile, std::list<Flight>& flights) noexcept(false);
    bool load_flights_to_DB(const std::filesystem::directory_entry& file, const std::string& airportCode) noexcept(false);
    std::list<Airport> Airports;
    bool loaded;
};

#endif