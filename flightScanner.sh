#! /bin/bash

# TODO: Need to add error handle examples: no curl apt, curl return [] empty, error in sed to awk commands

# -- Writed by Omer Start --
current_time=$(date +%s)
time_before_day=$(expr $current_time - 86400)  # 86400 - 24 hours
echo "Get last 24hours flights into flightsDB"
for airport in "$@" 
do
    mkdir -p "flightsDB/$airport"
    curl -s "https://opensky-network.org/api/flights/arrival?airport=$airport&begin=$time_before_day&end=$current_time" | sed 's/},{/},\n{/g' | cat > "flightsDB/$airport/$airport.arv"
    curl -s "https://opensky-network.org/api/flights/departure?airport=$airport&begin=$time_before_day&end=$current_time" | sed 's/},{/},\n{/g' | cat > "flightsDB/$airport/$airport.dst"

    # --- Writed by Nir Start ---
    keys=$(cat flightsDB/$airport/$airport.arv | awk -F'[:,]' 'BEGIN{ORS=""} {for(i=1;i<=NF;i+=2) if ($i in seen) next; else {seen[$i]; printf "%s%s", $i, (i==NF-1) ? "" : ","}} END{print "\n"}' | sed 's/[][]//g' | sed 's/,{"icao24",//' | sed 's/,$//' | tr -d '"' | sed 's/^{//')
    values=$(cat flightsDB/$airport/$airport.arv  | sed 's/\[//;s/\]//' | awk -F '[,:}]' '{print $2","$4","$6","$8","$10","$12","$14","$16","$18","$20","$22","$24}' | sed 's/"/ /g' | sed 's/ //g')
    echo "$keys"  > flightsDB/$airport/$airport.arv                                                                                                                                                      
    echo "$values" >> flightsDB/$airport/$airport.arv
    keys=$(cat flightsDB/$airport/$airport.dst | awk -F'[:,]' 'BEGIN{ORS=""} {for(i=1;i<=NF;i+=2) if ($i in seen) next; else {seen[$i]; printf "%s%s", $i, (i==NF-1) ? "" : ","}} END{print "\n"}' | sed 's/[][]//g' | sed 's/,{"icao24",//' | sed 's/,$//' | tr -d '"' | sed 's/^{//')
    values=$(cat flightsDB/$airport/$airport.dst  | sed 's/\[//;s/\]//' | awk -F '[,:}]' '{print $2","$4","$6","$8","$10","$12","$14","$16","$18","$20","$22","$24}' | sed 's/"/ /g' | sed 's/ //g')
    echo "$keys"  > flightsDB/$airport/$airport.dst                                                                                                                                                      
    echo "$values" >> flightsDB/$airport/$airport.dst
    # --- Writed by Nir End ---
done
echo "Done"
# -- Writed by Omer END --