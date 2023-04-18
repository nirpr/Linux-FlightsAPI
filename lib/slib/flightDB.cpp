#include "flightDB.h"
using namespace std;
bool FlightDatabase::load_db(const string &dataBasePath, const string &airportCode)
{
    string airportPathDB(dataBasePath); 
    // adding flightsDB directory ("\\" for windows and "/" for linux)
    airportPathDB.append("\\").append(airportCode).append("\\").append(airportCode); // Add to path .../airportCode/airportCode
    try
    {
        load_flights_to_DB(airportPathDB,".arv",airportCode);
        load_flights_to_DB(airportPathDB,".dst",airportCode);
        this->loaded = true;
    }
    catch(...)
    {
        cerr << "Error: in loading data to Airport "<< airportCode <<" from database" << endl;
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

const set<Flight> FlightDatabase::getAirplanes(const set<string>& icao24_required) const
{
    set<Flight> flights;
    for(auto& airport : this->Airports)
    {
        for(auto& flight : airport.get_flightsArv())
        {
            if( icao24_required.count( flight.get_icao24() ) );
                flights.insert(flight);
        }
        for(auto& flight : airport.get_flightsDpt())
        {
            if( icao24_required.count( flight.get_icao24() ) );
                flights.insert(flight);
        }
    }
    return flights;
}

bool FlightDatabase::load_flights_to_DB(const string& filePath, const string& postfix, const string& airportCode)
{
    string airportPathDB(filePath);
    airportPathDB.append(postfix);
    ifstream inFile;
    inFile.open(airportPathDB.c_str(),ifstream::in);
    // skip first line
    if (inFile.is_open())
    {
        string dummyString;
        getline(inFile, dummyString);
    }
    else 
    {
        inFile.close();
        throw;
    }
    list<Flight> flights;
    getFlightsFromFiles(inFile, flights);
    if (! this->get_airports_names().count(airportCode))
        this->Airports.push_back(Airport(airportCode));
    for(auto& flight : flights)
        if(postfix == ".arv")
            this->Airports.back().add_flightArv(flight);
        else if(postfix == ".dst")
            this->Airports.back().add_flightDpt(flight);  
        else
        {
            inFile.close();
            throw; // throw error for unknown postfix   
        }   
    this->Airports.back().changeDataLoadStatus(true);     
    inFile.close();
    return true;
}

void FlightDatabase::getFlightsFromFiles(ifstream& inFile, list<Flight>& flights)
{
    if(!inFile.is_open())
        throw;
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
        flights.push_back(Flight(icao24,callsign,origin,destination,stoi(departure_time), stoi(arrival_time)));
    }
}

