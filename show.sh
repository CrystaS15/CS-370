#!/bin/bash

# Name: Kristy Nguyen
# NSHE ID: 5006243601
# Section: 1003
# Lab: Process Context Switching

# Store PID of program
PID=$(pidof $1)

# Check for non-existent process
if [ ! $(pidof $1) ]; then
    echo -e "Error: non-existent process."

# Check for invalid parameters
elif [ "$#" -ne 1 ]; then
    echo -e "Error: invalid parameters."
    echo -e "Usage: ./show.sh <program>"

# Display context switches & user/system/total/start time
else
    # PID Information
    echo -e "Process "`cat /proc/$PID/status | awk '{if(NR==6) print $2}'`" Information"
    
    # Voluntary Context Switches
    echo -e `cat /proc/$PID/status | awk '{if(NR==56) print $1, $2}'`
    
    # Non-Voluntary Context Switches
    echo -e `cat /proc/$PID/status | awk '{if(NR==57) print $1, $2}'`
    
    # User Time
    USRTIME=`echo "scale=2; $(cat /proc/$PID/stat | cut -d' ' -f14) / $(getconf CLK_TCK)" | bc`
    echo -e "User Time: $USRTIME"
    
    # System Time
    SYSTIME=`echo "scale=2; $(cat /proc/$PID/stat | cut -d' ' -f14) / $(getconf CLK_TCK)" | bc`
    echo -e "System Time: $SYSTIME"
    
    # Total Time
    echo -e "Total time: " `echo | awk -v TOTALTIME=$USRTIME+$SYSTIME '{print TOTALTIME}' | bc`
    
    # Start Time
    echo -e "Start Time: "`ps -o start $PID | sed '1d'`
fi
