#include "flight.h"
using namespace std;

bool operator==(const Flight& lhs, const Flight& rhs)
{
    return lhs.get_icao24() == rhs.get_icao24() && lhs.get_arrival_time() == rhs.get_arrival_time()
            && lhs.get_callsign() == rhs.get_callsign() && lhs.get_destination() == rhs.get_destination()
            && lhs.get_origin() == rhs.get_origin();
}

bool operator==(const Flight& lhs, const string& icao24)
{
    return lhs.get_icao24().compare(icao24); 
}