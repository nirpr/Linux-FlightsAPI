Linux-Assignment-1
by: Omer Kriger - 207752536, Or Segal - 313151862, Nir Peretz - 318382850

Notes for assessment:
- Please make sure that the directory "flightsDB" is in the same directory as the makefile and .out files. (it should be automatic but just to make sure).
- If the user enters a wrong ICAO as parameter to the bash script the program will print an error and continue to the next ICAO.
- If there aren't any arrivals or departures from a certain airport then the program will delete the files .arv/.dst and if both are missing,
  it'll delete the airport directory.
- If There are directory in "flightsDB" and you run the bash script the program will add new airports and airport already in the database will overwrite with new data.
- After you'll enter "make clean" all the .out and .so and .o files will be deleted.
- The programs arguments are case sensitive, You should type Airports ICAO in uppercase, and Airplane ICAO24 lowercase.
- In reRun program, if script run without inputs all the database will be refresh to the last 24hours.
  if the scripts run with arguments at the end of running will be in the database only the airports from the arguments (the folder will overwrite).
- The programs should print warning information about missing .arv or .dst files in load the database, the print will be to the error output. 