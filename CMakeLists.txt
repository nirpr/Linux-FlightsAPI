cmake_minimum_required(VERSION 3.13) # CMake version check
project("FlightTracker" VERSION 1.1)

# CPP BUILD Settings
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Source files register
set(SOURCE_FILES
    lib/flightTracker.cpp
    lib/slib/airplane.cpp
    lib/slib/airport.cpp
    lib/slib/flight.cpp
    lib/slib/flightDB.cpp
    lib/slib/full_schedule.cpp
    lib/slib/reRun.cpp
    lib/slib/utility.cpp
)

add_executable(runMe.out ${SOURCE_FILES})