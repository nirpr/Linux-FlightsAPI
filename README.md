# CLI to print flights details from OpenSky Network
This project, created by Omer Kriger, Or Segal, and Nir Peretz, provides a command line interface for printing flight details from the OpenSky Network. The project is designed to run on a Linux environment.

This project leverages two Docker containers pulled from Docker Hub, managing them as separate processes that communicate with each other to perform different tasks. By using Docker, the program ensures a consistent runtime environment, simplifies dependency management, and enhances functionality through efficient process management. This design allows the program to handle multiple tasks simultaneously, resulting in improved performance and efficient execution.

# Installation and Setup
To use this project, follow the steps outlined below:  

### 1. Clone the repository  
 ``` bash
git clone https://github.com/nirpr/Linux-FlightsAPI.git
cd Linux-FlightsAPI
```
### 2. Run the initialization Script   
Ensure you are in the root directory and execute the following command (you may need sudo depending on your Docker setup):  
``` bash
bash init.sh
```
### 3. Set up the database   
In the menu, select "Fetch airports data" (option 1).   
This step must be completed before using any other features.   
### 4. Explore the available options   
After the database is set up, you can choose different actions from the menu.

# Docker Containers
This project utilizes two different Docker containers that are pulled from Docker Hub:

Container 1: [flights_service](https://hub.docker.com/repository/docker/nirp13/flights_service/general)  
Container 2: [DB_service](https://hub.docker.com/repository/docker/nirp13/db_service/general)   
These containers are integral to the functionality of the program and are automatically managed during the setup process.

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
