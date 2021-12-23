#!/bin/sh
gnome-terminal -e  " roslaunch vehicle_control vehicle_control.launch " &
sleep 3
gnome-terminal  -e  " roslaunch vehicle_simulator vehicle_simulator.launch "
