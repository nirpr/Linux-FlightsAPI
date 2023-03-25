#! /bin/bash

current_time=$(date +%s)
time_before_day=$(expr $current_time - 86400)  # 8644 - 24 hours
echo "begin time is $time_before_day"
echo "end time is $current_time"
echo "Get last 24hours flights into flightsDB"
for airport in "$@" 
do
    mkdir "flightsDB/$airport"
    echo "curl -s \"https://opensky-network.org/api/flights/arrival?airport=$airport&begin=$time_before_day&end=$current_time\""
    curl -s "https://opensky-network.org/api/flights/arrival?airport=$airport&begin=$time_before_day&end=$current_time" | sed 's/},{/},\n{/g' | cat > "flightsDB/$airport/$airport.arv"
    echo "curl -s \"https://opensky-network.org/api/flights/departure?airport=$airport&begin=$time_before_day&end=$current_time\""
    curl -s "https://opensky-network.org/api/flights/departure?airport=$airport&begin=$time_before_day&end=$current_time" | sed 's/},{/},\n{/g' | cat > "flightsDB/$airport/$airport.dst"
done
echo "Done"