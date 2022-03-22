#!/bin/sh

# gnome-terminal -e  " roslaunch vehicle_control manual_control.launch " &
# sleep 1
gnome-terminal  -e  " roslaunch vehicle_simulator vehicle_simulator.launch "  &
sleep 2
gnome-terminal -e  " roslaunch vehicle_control vehicle_control.launch "
