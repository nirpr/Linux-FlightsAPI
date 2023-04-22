#include "airport.hpp"
using namespace std;

bool operator==(const Airport& lhs, const Airport& rhs)
{
    return lhs.get_name() == rhs.get_name();
}

bool Airport::changeDataLoadStatus(bool newStatus)
{
    return this->ariportDataLoaded = newStatus;
}

void Airport::add_flightDpt(const Flight& flight) 
{
    flightsDpt.push_back(flight);
    this->num_flights++;
    this->num_flightsDpt++;
}

void Airport::add_flightArv(const Flight& flight) 
{
    flightsArv.push_back(flight);
    this->num_flights++;
    this->num_flightsArv++;
}


