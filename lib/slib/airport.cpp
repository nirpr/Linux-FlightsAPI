#include "airport.h"
using namespace std;

bool operator==(const Airport& lhs, const Airport& rhs)
{
    return lhs.get_name() == rhs.get_name();
}

bool Airport::changeDataLoadStatus(bool newStatus)
{
    return this->ariportDataLoaded = newStatus;
}


