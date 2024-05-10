#!/bin/bash

# System Information Option, sys
if [ "$1" == "sys" ]; then
    echo -e "-------------------------------System Information----------------------------"
    # Hostname
    echo -e "Hostname:\t\t"`hostname`
    # Uptime
    echo -e "uptime:\t\t\t"`uptime -p | sed 's/up //'`
    # Manufacturer
    echo -e "Manufacturer:\t\t"`cat /sys/class/dmi/id/chassis_vendor`
    # Product name
    echo -e "Product name:\t\t"`cat /sys/class/dmi/id/product_name`
    # Version
    echo -e "Version:\t\t"`cat /sys/class/dmi/id/product_version`
    # Machine Type
    echo -e "Machine Type:\t\t"`vserver=$(lscpu | grep Hypervisor | wc -l); if [ $vserver -gt 0 ]; then echo "VM"; else echo "Physical"; fi`
    # Operating System
    echo -e "Operating System:\t"`lsb_release -d | sed 's/Description:\t'//`
    # Kernel Version 
    echo -e "Kernel:\t\t\t"`uname -s`
    # Architecture
    echo -e "Architecture:\t\t"`arch`
    # Processor Name
    echo -e "Processor Name:\t\t"`awk -F':' '/^model name/ {print $2}' /proc/cpuinfo | uniq | sed -e 's/^[ \t]*//'`
    # Active User
    echo -e "Active User:\t\t"`users`
    # Main System IP Address
    echo -e "Main System IP:\t\t"`hostname -I`

#Memory Option, mem
elif [ "$1" == mem ]; then
    echo -e "-------------------------------CPU/Memory Usage------------------------------"
    # Displays the CPU/Memory Usage (free)
    free
    echo

    # Display memory usage percentage
    printf "%s%.2f%s" "Memory Usage:    " $(free | awk 'NR==2 {mem_used = $3/$2; print mem_used*100;}') "%"
    echo

    # Display swap usage percentage
    printf "%s%.2f%s" "Swap Usage:      " $(free | awk 'NR==2 {mem_used = $3/$2; print mem_used*100;}') "%"
    echo

    # Display CPU usage percentage
    echo -e "CPU Usage:       "`cat /proc/stat | awk '/cpu/{printf("%.2f%%\n"), ($2+$4)*100/($2+$4+$5)}' | awk '{print $0}' | head -1`

elif [ "$1" == disk ]; then
    echo -e "-------------------------------Disk Usage-------------------------------"
    
    # Display disk usage
    df -h | awk '$NF=="/"{printf "Disk Usage: %s\t\t\n\n", $5}'

    # Display file systems
    df -h

# Error handling - Incorrect option provided
elif [ "$1" == "" ]; then
    echo -e "Usage: sysInfo <sys|mem|disk>"

# Error handling - Invalid option provided
else
    echo -e "Error, invalid parameter."
fi
