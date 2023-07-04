#! /bin/bash

sudo docker build -t db_service:1.1 -f dockerfile.DB .

sudo docker build -t flights_service:1.1 -f dockerfile.flights .


