# What is this?
UAV tunnel navigation control, running on ROS. Ad hoc network and enclosed environments for Gazebo are included.

---

# Environments
## Y-tunnel
![](./img/simu_Y.png)
A tunnel with two branches. It was manually modeled by using Blender.

## Indian Tunnel
![](./img/simu_Indian.png)
The Indian lava tube, based on [the ranging sensor data provided by CMU](http://www.frc.ri.cmu.edu/projects/NIAC_Caves/). The details of generation of this 3D model can be found here:
https://github.com/tidota/poisson-sampling

---

# Navigation Algorithms/Code
## Reactive Agent
The program is composed of several nodes each of which represent a node to take care of a specific behavior.
Every node takes the same sensory data and makes its own decision or control outputs (geometry_msgs/Twist).
They are connected with each other by ROS topics, consisting a tree.
All messages generated by them go toward to the root node: main_control, which finally generates /cmd_vel.

A node has two options to generate its output: combine and pass the control values given by the other nodes toward the root, or block them and pass its own.
When specific conditions to its corresponding behavior are satisfied, it blocks the messages from the other nodes and passes its own toward the root.
Otherwise, it combines and passes the control values generated by the others.
This mechanism lets a specific behavior appear only in its assumed situation. For example, the node, altitude_adjustment, connects to the one, obstacle_avoidance. The obstacle_avoidance node passes the message from others only if there is no impending obstacles to avoid immediately; the altitude_adjustment node can generate its behavior without impending obstacles.

At the moment (as of Dec 25, 2017), it contains six nodes.
- **obstacle_avoidance**: avoids impending obstacles, assumes nothing.
- **altitude_adjustment**: keeps the altitude in the middle of the vertical line, assumes that there is no impending obstacles.
- **turning_around**: turns the robot around so that it can avoid the wall in front of it.
- **steering**: adjusts the heading so that the robot's right side faces toward the wall.
- **staying_on_the_middle_line**: adjusts the horizontal position in the tube.
- **going_straight**: moves the robot forward.

![diagram of nodes](./img/nodes_reactive.png)

## Other approaches?
TBA

---

# Installation

TODO: I will upgrade the package so they can run on ROS Melodic.

## Required Environment
- OS: Ubuntu 16.04
- ROS version: Kinetic

## Setup
This package requires hector-quadrotor. Check [the details](SETUP.md).

## Installation of this package
```
cd ~/catkin/src
git clone git@github.com:tidota/quadrotor-tunnel-nav.git
cd ..
catkin_make
```

---

# How to run
You need to launch two parts in separated terminal windows: Gazebo (simulation environment) and controller.

## Gazebo:

Y-tunnel
```
roslaunch quadrotor_tunnel_nav uav_Y-tunnel.launch
```
Indian Tunnel
```
roslaunch quadrotor_tunnel_nav uav_IndianTunnel.launch
```

## Controller:
The hector-quadrotor (built from the source) apparently [disables the motors in default](https://answers.ros.org/question/256590/hector-quadcopter-not-responding-to-cmd_vel-messages/) and it is required to enable them.
```
rosservice call /enable_motors true
roslaunch quadrotor_tunnel_nav control.launch
```
Then, you can see a quadrotor flying inside a tunnel in the simulator window.

---

# Ad Hoc Network

![environment for ad hoc network simulation](img/adhoc_env.jpg)

This package also simulates Ad Hoc network composed of multiple UAVs based on [AllNet](http://alnt.org/). Please note that it will require high machine power. A desktop PC or workstation is recommended.
This part of the package was tested and used on HP Z620.

In a rectangular space, 10 UAVs fly around by the similar reactive controller mentioned above. The configuration of robot formation and network is defined in `config/adhoc/robots.yaml` and `config/adhoc/simconfig.yaml`.

The communication is managed by a world plugin `AdHocNetPlugin` and a model plugin `AdHocClientPlugin`. The model plugin is applied to each robot and communicates with the world plugin. The world plugin checks the robot's location and determines to pass a packet based on the communication range.

As of October 10, 2018, they are just broadcasting packets, but epidemic routing will be implemented in the future.

Before running the simulation, the following command must run so that multiple nodes can open sockets to communicate with each other.

```
ulimit -n 9000
```

Then, in your catkin workspace, build the program and run it as follows.

```
. /opt/ros/kinetic/setup.bash
catkin_make
. devel/setup.bash
roslaunch quadrotor_tunnel_nav I-tunnel-40x10.launch
```

---

# Memorandum
[Misc notes of ROS commands](MEMORANDUM.md)
