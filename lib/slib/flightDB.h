#ifndef __FLIGHT_DB_H
#define __FLIGHT_DB_H

#include "airport.h"
#include "flight.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream> // Require for stringstream
#include <stdexcept>
#include <sys/stat.h>   // Required for functions in Linux
#include <system_error> // ERROR CODES
#include <zip.h>

#define DB_PATH "./flightsDB"
#define DB_PREMISSIONS S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
enum class std_error : unsigned long
{
    _Success = 0,                     // #define ERROR_SUCCESS                    0L
    _Invalid_function = 1,            // #define ERROR_INVALID_FUNCTION           1L
    _File_not_found = 2,              // #define ERROR_FILE_NOT_FOUND             2L
    _Path_not_found = 3,              // #define ERROR_PATH_NOT_FOUND             3L
    _Access_denied = 5,               // #define ERROR_ACCESS_DENIED              5L
    _Not_enough_memory = 8,           // #define ERROR_NOT_ENOUGH_MEMORY          8L
    _No_more_files = 18,              // #define ERROR_NO_MORE_FILES              18L
    _Sharing_violation = 32,          // #define ERROR_SHARING_VIOLATION          32L
    _Not_supported = 50,              // #define ERROR_NOT_SUPPORTED              50L
    _File_exists = 80,                // #define ERROR_FILE_EXISTS                80L
    _Invalid_parameter = 87,          // #define ERROR_INVALID_PARAMETER          87L
    _Insufficient_buffer = 122,       // #define ERROR_INSUFFICIENT_BUFFER        122L
    _Invalid_name = 123,              // #define ERROR_INVALID_NAME               123L
    _Directory_not_empty = 145,       // #define ERROR_DIR_NOT_EMPTY              145L
    _Already_exists = 183,            // #define ERROR_ALREADY_EXISTS             183L
    _Filename_exceeds_range = 206,    // #define ERROR_FILENAME_EXCED_RANGE       206L
    _Directory_name_is_invalid = 267, // #define ERROR_DIRECTORY                  267L
    _Max = ~0UL                       // sentinel not used by Win32
};

class FlightDatabase
{
        public:
    enum class DataParameters
    {
        icao24,
        callsign,
        origin,
        destination,
        departure_time,
        arrival_time,
        totalParm
    };
    enum class Directions
    {
        arriving,
        destinations,
        both
    };
    FlightDatabase(bool loadfromZip) noexcept(false);
    void load_DB_from_folder(bool reRun) noexcept(false);
    bool load_db(const std::string &airportCode) noexcept(false);
    const std::list<Airport> get_flights_by_airport_name(const std::set<std::string> &airport_name, int ToFromBoth) const;
    std::set<std::string> get_airports_names() const;
    const std::set<Flight> getAirplanes(const std::string &icao24) const;
    const std::set<Flight> getAirplanes(const std::set<std::string> &icao24_required) const;
    static void unzipDB() noexcept(false);
    static void zipDB() noexcept(false);
    static void writeStringToFile(const std::string &str, const std::string &fileName, mode_t permissions) noexcept(false);
    // getter
    bool getLoadStatus() { return loaded; }

        private:
    void getFlightsFromFiles(std::ifstream &inFile, std::list<Flight> &flights) noexcept(false);
    bool load_flights_to_DB(const std::filesystem::directory_entry &file, const std::string &airportCode) noexcept(false);
    std::list<Airport> Airports;
    bool loaded;
};

#endif