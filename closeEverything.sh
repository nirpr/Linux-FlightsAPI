#! /bin/bash
sudo docker stop $(sudo docker ps -q)
sudo docker rmi -f $(sudo docker images -q)
# docker rm -f $(docker ps -aq)