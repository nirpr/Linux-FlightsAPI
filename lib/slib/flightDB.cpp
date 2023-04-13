#include "flightDB.h"
using namespace std;
bool FlightDatabase::load_db(const string &dataBasePath, const string &airportCode)
{
    string airportPathDB(dataBasePath);
    airportPathDB.append("/").append(airportCode).append("/").append(airportCode); // Add to path .../airportCode/airportCode
    try
    {
        load_flights_to_DB(airportPathDB,".arv",airportCode);
        load_flights_to_DB(airportPathDB,".dst",airportCode);
    }
    catch(...)
    {
        cerr << "Error: in loading data to Airport "<< airportCode <<" from database" << endl;
    }
}

bool FlightDatabase::load_flights_to_DB(const string& filePath, const string& postfix, const string& airportCode)
{
    string airportPathDB(filePath);
    airportPathDB.append(postfix);
    ifstream inFile;
    inFile.open(airportPathDB.c_str(),ifstream::in);
    if (inFile.fail())
    {
        throw;
    }
    else
    {
        list<Flight> flights = getFlightsFromFiles(inFile);
        this->Airports.push_back(Airport(airportCode));
        for(auto& flight : flights)
            if(postfix == ".arv")
                this->Airports.back().add_flightArv(move(flight));
            else
                this->Airports.back().add_flightDpt(move(flight));                
    }
}

list<Flight>&& FlightDatabase::getFlightsFromFiles(ifstream& inFile)
{
    list<Flight> flights;
    string icao24, callsign, origin, destination, departure_time, arrival_time;
    while(inFile.good())
    {
        getline(inFile,icao24,',');
        getline(inFile,departure_time,',');
        getline(inFile,origin,',');
        getline(inFile,arrival_time,',');
        getline(inFile,destination,',');
        getline(inFile,callsign,',');
        // maybe need get \n
        if(inFile.fail() || stoi(departure_time) || stoi(arrival_time))
        {
            throw 0;
        }
        flights.push_back(Flight(icao24,callsign,origin,destination,stoi(departure_time), stoi(arrival_time)));
    }
    return move(flights);
}

