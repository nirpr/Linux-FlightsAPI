Linux-Assignment-1
by: Omer Kriger - 207752536, Or Segal - 313151862, Nir Peretz - 318382850

Notes for assessment:
- Please make sure that the directory "flightsDB" is in the same directory as the makefile and .out files. (it should be automatic but just to make sure).
- If the user enters a wrong ICAO as parameter to the bash script the program will print an error and continue to the next ICAO.
- If there aren't any arrivals or departures from a certain airport then the program will delete the files .arv/.dst and if both are missing,
  it'll delete the airport directory.
- after you'll enter "make clean" all the .out and .so and .o files will be deleted.