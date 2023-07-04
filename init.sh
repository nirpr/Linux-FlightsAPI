#! /bin/bash

sudo docker run -d --restart unless-stopped -v /tmp/flights_pipe:/tmp/flights_pipe db_service:1.1

sudo docker run -it --restart unless-stopped -v /tmp/flights_pipe:/tmp/flights_pipe flights_service:1.1


