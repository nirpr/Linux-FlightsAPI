#include "flightDB.h"
using namespace std;
namespace fs = std::filesystem;

bool FlightDatabase::load_db(const string &dataBasePath, const string &airportCode) noexcept(false)
{
    string airportPathDB(dataBasePath); 
    // adding flightsDB directory ("\\" for windows and "/" for linux)
    airportPathDB.append("/").append(airportCode); // Add to path .../airportCode // Folder Path
    bool arv = false, dst = false;
    try
    {
        fs::directory_iterator dict_itr(airportPathDB);
        for (const auto& itr : dict_itr)
        {
            try
            {
                if (itr.path().extension().string() == ".arv")
                    arv = true;
                else if (itr.path().extension().string() == ".dpt")
                    dst = true;
                load_flights_to_DB(itr, airportCode);
            }
            catch (const std::exception& e)
            {
                cerr << e.what() << " " << itr.path().filename().string() << " from database" << endl;
            }
        }
        if (arv == false)
        {
            cerr << "There are no arrivals to " << airportCode << " Airport." << endl;
        }
        if (arv == false)
        {
            cerr << "There are no departures to " << airportCode << " Airport." << endl;
        }
    }
    catch (const fs::filesystem_error& e)
    {
        if (e.code().value() == (int)__std_win_error::_File_not_found || e.code().value() == (int)__std_win_error::_Path_not_found)
        {
            cerr << "Error: Airport " << airportCode << " not found in the Database." << endl;
            return false;
        }
        else if (e.code().value() == (int)__std_win_error::_File_not_found)
        {
            cerr << "Error: Access Denied to " << airportCode << " from the Database" << endl;
            return false;
        }
        else
        {
            cerr << "Error: Unknown error in reading " << airportCode << " from the Database" << endl;
        }
    }
    return true;
}

set<string> FlightDatabase::get_airports_names() const
{
    set<string> airportsNames;
    for (auto& airport : this->Airports)
        airportsNames.insert(airport.get_name());
    return airportsNames;
}

const set<Flight> FlightDatabase::getAirplanes(const string& icao24) const
{
    set<Flight> flights;
    for(auto& airport : this->Airports)
    {
        for(auto& flight : airport.get_flightsArv())
        {
            if(flight == icao24)
                flights.insert(flight);
        }
        for(auto& flight : airport.get_flightsDpt())
        {
            if(flight == icao24)
                flights.insert(flight);
        }
    }
    return flights;
}

const list<Airport> FlightDatabase::get_flights_by_airport_name(const set<string>& airports_names, int ToFromBoth) const
{
    list<Airport> airportsRequired;
    for(auto& airport : this->Airports)
    {
        if(airports_names.count(airport.get_name()))
            airportsRequired.push_back(airport);
    }
    return airportsRequired;
}

const set<Flight> FlightDatabase::getAirplanes(const set<string>& icao24_required) const
{
    set<Flight> flights;
    for(auto& airport : this->Airports)
    {
        for(auto& flight : airport.get_flightsArv())
        {
            if(icao24_required.count(flight.get_icao24()) );
                flights.insert(flight);
        }
        for(auto& flight : airport.get_flightsDpt())
        {
            if(icao24_required.count( flight.get_icao24() ) );
                flights.insert(flight);
        }
    }
    return flights;
}

bool FlightDatabase::load_flights_to_DB(const fs::directory_entry& file, const std::string& airportCode) noexcept(false)
{
    ifstream inFile;
    inFile.open(file.path().string(), ios::in);
    if (inFile.is_open())
    {
        string dummyString;
        getline(inFile, dummyString);   // skip first line
    }
    else 
    {
        inFile.close();
        throw logic_error("Error: Something gone wrong in the try to read,");
    }
    list<Flight> flights;
    try
    {
        getFlightsFromFiles(inFile, flights);
        if (!this->get_airports_names().count(airportCode))
            this->Airports.push_back(Airport(airportCode));
        for (auto& flight : flights)
            if (file.path().extension() == ".arv")
                this->Airports.back().add_flightArv(flight);
            else if (file.path().extension() == ".dst")
                this->Airports.back().add_flightDpt(flight);
            else
            {
                inFile.close();
                throw logic_error("Error: file suffix without handling,");  
            }
        this->Airports.back().changeDataLoadStatus(true);
    }
    catch (const std::exception& e)
    {
        inFile.close();
        throw e;
    }
    inFile.close();
    return true;
}

void FlightDatabase::getFlightsFromFiles(ifstream& inFile, list<Flight>& flights) noexcept(false)
{
    if(!inFile.is_open())
        throw logic_error("Error: Something gone wrong in try to reading the file.");
    string line;
    string icao24, callsign, origin, destination, departure_time, arrival_time;
    while(getline(inFile, line))
    {
        stringstream ss(line);
        getline(ss,icao24,',');
        getline(ss,departure_time,',');
        getline(ss,origin,',');
        getline(ss,arrival_time,',');
        getline(ss,destination,',');
        getline(ss,callsign,',');
        flights.push_front(Flight(icao24,callsign,origin,destination,stoi(departure_time), stoi(arrival_time)));
    }
}

