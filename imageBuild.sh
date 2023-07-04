#! /bin/bash

mkdir build  
cd build  
cmake ..  
make  
cd ..  

sudo docker build -t db_service:2.0 -f dockerfile.DB .
sudo docker build -t flights_service:2.0 -f dockerfile.flights .


