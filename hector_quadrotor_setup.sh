#! /bin/bash

# install dependancies for hector quadrotor
sudo apt-get install ros-kinetic-hardware-interface
sudo apt-get install ros-kinetic-controller-interface
sudo apt-get install ros-kinetic-control-toolbox
sudo apt-get install ros-kinetic-hector-gazebo-plugins
sudo apt-get install ros-kinetic-gazebo-ros-control
sudo apt-get install ros-kinetic-hector-pose-estimation
sudo apt-get install ros-kinetic-hector-sensors-description
sudo apt-get install ros-kinetic-message-to-tf

# install additional dependancies
sudo apt-get install ros-kinetic-hector-gazebo-worlds ros-kinetic-octomap-ros ros-kinetic-mrpt-bridge

sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
sudo apt-get update
sudo apt-get install libignition-transport4*
