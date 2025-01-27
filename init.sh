#! /bin/bash
sudo docker pull nirp13/db_service:2.1
sudo docker pull nirp13/flights_service:2.1

sudo docker run -d -v /tmp/flights_pipe:/tmp/flights_pipe nirp13/db_service:2.1
sudo docker run -it -v /tmp/flights_pipe:/tmp/flights_pipe nirp13/flights_service:2.1