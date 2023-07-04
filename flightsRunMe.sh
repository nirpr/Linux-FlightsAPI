#!/bin/bash

# Run the runMeDB.out process
./runMeDB.out
# Capture the exit status
exit_status=$?
# Check the exit status
if [ $exit_status -ne 0 ]; 
then
    echo "runMeDB.out exited with a non-zero status: $exit_status. Terminating the Docker container."
    exit 1
else
    echo "runMeDB.out exited with a zero status."
    exit 0
fi

