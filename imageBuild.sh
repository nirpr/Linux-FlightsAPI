#! /bin/bash

mkdir build  
cd build  
cmake ..  
make  
cd ..  

sudo docker build -t nirp13/db_service:2.1 -f dockerfile.DB .
sudo docker build -t nirp13/flights_service:2.1 -f dockerfile.flights .


