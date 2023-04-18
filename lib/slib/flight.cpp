#include "flight.h"
using namespace std;

bool operator==(const Flight& lhs, const Flight& rhs)
{
    if (lhs < rhs)
        return false;
    else
        return !(rhs < lhs);
}

bool operator<(const Flight& lhs, const Flight& rhs)
{
    if (lhs.icao24 != rhs.icao24)
        return lhs.icao24 < rhs.icao24;
    else if (lhs.callsign != rhs.callsign)
        return lhs.callsign < rhs.callsign;
    else if (lhs.origin != rhs.origin)
        return lhs.origin < rhs.origin;
    else if (lhs.destination != rhs.destination)
        return lhs.destination < rhs.destination;
    else if (lhs.departure_time != rhs.departure_time)
        return lhs.departure_time < rhs.departure_time;
    else
        return lhs.arrival_time < rhs.arrival_time;
}

bool operator>(const Flight& lhs, const Flight& rhs)
{
    return rhs < lhs;
}

bool operator==(const Flight& lhs, const string& icao24)
{
    return lhs.get_icao24().compare(icao24); 
}