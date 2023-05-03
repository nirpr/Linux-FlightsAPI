# Interface to print flights details from OpenSky Network
the project was created and meant to run on a Linux environment.
by: Omer Kriger, Or Segal, Nir Peretz.

STEPS for using:
1. Clone repository to folder
2. run in terminal "make" command to build .out files (make sure you are in the repo root directory).
3. Run the bash script "flightScanner.sh" or "reRun.out" with Airport ICAO as input arguments.
4. Now you can choose which program to run: airplane.out, full_schedule and arrivals.

General Notes:
- Oeration system: Linux.
- C++ Version: C++17 or above.
- Compiler: G++11 or above.
- The program uses the filesystem library, so please make sure the compilation runs on c++ 17 or above.
- The directory "flightsDB" should be in the same directory as the makefile and .out files.
- To execute the program you need premissions to read, write and execute.

Program behavior notes:
- If the user enters a wrong ICAO as parameter to the bash script the program will print an error and continue to the next ICAO.
- If there aren't any arrivals or departures from a certain airport then the program will delete the files .arv/.dst and if both are missing,
  it'll delete the airport directory.
- If There are directories in "flightsDB" and you run the bash script the program will add new airports and airport that are already in the database will be overwritten   with new data.
- After you'll enter "make clean", all the .out and .so and .o files will be deleted.
- The programs arguments are case sensitive, You should type Airports ICAO in uppercase, and Airplane ICAO24 lowercase.
- In the "reRun" program, if the script runs without inputs all the database will be refreshed to the last 24hours.
  if the scripts runs with arguments, the database will contain only the airports from the arguments (the folder will be overwritten). 
- The programs should print warning information about missing .arv or .dst files in load the database, the print will be to the error output. 
