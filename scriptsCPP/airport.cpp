#include "airport.h"
using namespace std;

Airport::Airport(string name) : name(name), num_flights(0) {}

string Airport::get_name() const { return name; }
int Airport::get_num_flights() const { return num_flights; }
const vector<Flight>& Airport::get_flights() const { return flights; }

void Airport::add_flight(const Flight& flight) {
    flights.push_back(flight);
    num_flights++;
}