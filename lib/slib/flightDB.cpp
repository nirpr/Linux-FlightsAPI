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
        this->loaded = true;
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

    list<Flight> flights = getFlightsFromFiles(inFile);
    this->Airports.push_back(Airport(airportCode));
    for(auto& flight : flights)
        if(postfix == ".arv")
            this->Airports.back().add_flightArv(move(flight));
        else if(postfix == ".dst")
            this->Airports.back().add_flightDpt(move(flight));  
        else
        {
            inFile.close();
            throw; // throw error for unknown postfix   
        }   
    this->Airports.back().changeDataLoadStatus(true);     
    inFile.close();
}

list<Flight>&& FlightDatabase::getFlightsFromFiles(ifstream& inFile)
{
    if(!inFile.is_open())
        throw;
    list<Flight> flights;
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
    return move(flights);
}

