# Interface to print flights details from OpenSky Network
This project, created by Omer Kriger, Or Segal, and Nir Peretz, provides an interface for printing flight details from the OpenSky Network. The project is designed to run on a Linux environment.

This project leverages processes and communication between them to enhance its functionality. By utilizing this approach, the program is able to effectively handle multiple tasks simultaneously, resulting in improved performance and efficient execution.

# STEPS for using:  
To use this project, follow the steps outlined below:  

1. Clone repository  
2. Ensure you are in the root directory, then copy the following code to the terminal:  
   "mkdir build  
    cd build  
    cmake ..  
    make  
    cd ..  
    ./runMe.out"  
3. Press "UpdateDB" (number 4 in the menu) to create the database. This should be the first action you take.  
4. Now you can choose different actions from the menu.  

# General Notes:
Please take note of the following general information: 

-Operation system: Linux.  
-C++ Version: C++17 or above.  
-Compiler: G++11 or above.  
-The program uses the filesystem library, so please make sure the compilation runs on c++ 17 or above.  
-The directory "flightsDB" should be in the root directory.  
-To execute the program you need premissions to read, write and execute.  

# Program behavior Notes: 
Consider the following notes about the behavior of the program:  

-If there aren't any arrivals or departures from a certain airport then the program will delete the files .arv/.dst and if both are missing, it'll delete the airport directory.  
-The programs arguments are case sensitive, You should type Airports ICAO in uppercase, and Airplane ICAO24 lowercase.  
-In "UpdateDB", if there aren't any inputs, the database will be refreshed to the last 24hours. if arguments were entered, the database will contain only the airports from the arguments (the folder will be overwritten).  
