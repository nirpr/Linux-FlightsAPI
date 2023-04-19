#! /bin/bash

# TODO: Need to add error handle examples: no curl apt, curl return [] empty, error in sed to awk commands

# -- Written by Omer Start --
if [ $# -eq 0 ]; then
    echo "Error: No airport codes provided"
    exit 1
fi
current_time=$(date +%s)
time_before_day=$(expr $current_time - 86400)  # 86400 - 24 hours
echo "Get last 24hours flights into flightsDB"
for airport in "$@" 
do
    if ! curl -s --fail "https://opensky-network.org/api/flights/arrival?airport=$airport&begin=$time_before_day&end=$current_time" >/dev/null; then
    echo "Error: Invalid airport code - $airport"
    continue
    fi
    if ! curl -s --fail "https://opensky-network.org/api/flights/departure?airport=$airport&begin=$time_before_day&end=$current_time" >/dev/null; then
    echo "Error: Invalid airport code - $airport"
    continue
    fi

    mkdir -p "flightsDB/$airport"
    curl -s "https://opensky-network.org/api/flights/arrival?airport=$airport&begin=$time_before_day&end=$current_time" | sed 's/},{/},\n{/g' | cat > "flightsDB/$airport/$airport.arv"
    curl -s "https://opensky-network.org/api/flights/departure?airport=$airport&begin=$time_before_day&end=$current_time" | sed 's/},{/},\n{/g' | cat > "flightsDB/$airport/$airport.dst"

    # --- Written by Nir Start ---
    #arv file manipulation:
    keys=$(cat flightsDB/$airport/$airport.arv | awk -F'[:,]' '{print $1, $3, $5, $7, $9, $11}' | sort | uniq | head -1 | sed -e 's/"//g' -e 's/[{[]//g' -e 's/]/,/g' -e 's/ /,/g')
    values=$(cat flightsDB/$airport/$airport.arv  | sed 's/\[//;s/\]//' | awk -F '[,:}]' '{print $2","$4","$6","$8","$10","$12}' | sed 's/"/ /g' | sed 's/ //g')
    echo -e "$keys\n$values" > flightsDB/$airport/$airport.arv
    #check if file empty and then delete if empty
    if grep -q "^[,]*$" "flightsDB/$airport/$airport.arv"; then
    echo "No arrivals found for $airport in the last 24 hours."
    rm flightsDB/$airport/$airport.arv
    fi
    #dst file manipulation:
    keys=$(cat flightsDB/$airport/$airport.dst | awk -F'[:,]' '{print $1, $3, $5, $7, $9, $11}' | sort | uniq | head -1 | sed -e 's/"//g' -e 's/[{[]//g' -e 's/]/,/g' -e 's/ /,/g')
    values=$(cat flightsDB/$airport/$airport.dst  | sed 's/\[//;s/\]//' | awk -F '[,:}]' '{print $2","$4","$6","$8","$10","$12}' | sed 's/"/ /g' | sed 's/ //g')
    echo -e "$keys\n$values" > flightsDB/$airport/$airport.dst
    #check if file empty and then delete if empty
    if grep -q "^[,]*$" "flightsDB/$airport/$airport.dst"; then
    echo "No departures found for $airport in the last 24 hours."
    rm flightsDB/$airport/$airport.dst
    fi
    #check if directory is empty and then delete if it is.
    if [ -n "$(find flightsDB/$airport -maxdepth 0 -type d -empty)" ]; then
    echo "There are no departures or arrivals to/from $airport, deleting directory."
    rmdir "flightsDB/$airport"
    fi
    # --- Written by Nir End ---
done
echo "Done"
# -- Written by Omer END --