cmake_minimum_required(VERSION 3.13) # CMake version check
project(FlightTracker VERSION 1.2)

# CPP BUILD Settings
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Source files register
file(GLOB_RECURSE SOURCE_FILES_DB
    "lib/dbService.cpp"
    "lib/slib/*.cpp"
    "lib/dlib/*.cpp"
)

# Source files register
file(GLOB_RECURSE SOURCE_FILES_FLIGHTS
    "lib/FlightsService.cpp"
    "lib/slib/*.cpp"
    "lib/flib/*.cpp"
)

add_executable(runMeDB.out ${SOURCE_FILES_DB})
add_executable(runMeFlights.out ${SOURCE_FILES_FLIGHTS})


# Set the output directory to the project root directory.
set_target_properties(runMeDB.out PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(runMeFlights.out PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Find the libzip library
find_library(LIBZIP zip REQUIRED)

# Link the libzip library to your project
target_link_libraries(runMeDB.out PRIVATE ${LIBZIP})
