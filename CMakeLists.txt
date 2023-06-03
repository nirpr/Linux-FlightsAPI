cmake_minimum_required(VERSION 3.13) # CMake version check
project(FlightTracker VERSION 1.1)

# CPP BUILD Settings
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Source files register
set(SOURCE_FILES
    lib/flightTracker.cpp
    lib/slib/airport.cpp
    lib/slib/flightDB.cpp
    lib/slib/flight.cpp
    lib/slib/utility.cpp
    lib/slib/airplane.cpp
    lib/slib/arrivals.cpp
    lib/slib/full_schedule.cpp
    lib/slib/reRun.cpp
)

add_executable(runMe.out ${SOURCE_FILES})

# Set the output directory to the project root directory.
set_target_properties(runMe.out PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Find the libzip library
find_library(LIBZIP zip REQUIRED)

# Link the libzip library to your project
target_link_libraries(runMe.out PRIVATE ${LIBZIP})
