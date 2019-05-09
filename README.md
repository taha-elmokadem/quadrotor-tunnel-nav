# What is this?
UAV tunnel navigation control, running on ROS. Ad hoc network and enclosed environments for Gazebo are included.

This is [a demo video of UAV flight by the behavior-based agent](https://drive.google.com/file/d/1hmi2ZhFWjk8G8RE5_dyIA2ykBBFXvZxI/view).

---

# Environments
## Y-tunnel
![](./img/simu_Y.png)
A tunnel with two branches. It was manually modeled by using Blender.

To run the simulated environment with a single UAV,
```
roslaunch quadrotor_tunnel_nav single_uav_Y-tunnel.launch
```
To run the simulated environment with two UAVs,
```
roslaunch quadrotor_tunnel_nav multi_uav_Y-tunnel.launch
```

## Indian Tunnel
![](./img/simu_Indian.png)
The Indian lava tube, based on [the ranging sensor data provided by CMU](http://www.frc.ri.cmu.edu/projects/NIAC_Caves/). The details of generation of this 3D model can be found here:
https://github.com/tidota/poisson-sampling

To run the simulated environment with a single UAV,
```
roslaunch quadrotor_tunnel_nav single_uav_IndianTunnel.launch
```
To run the simulated environment with two UAVs,
```
roslaunch quadrotor_tunnel_nav multi_uav_IndianTunnel.launch
```

---

# Navigation Algorithms/Code

The hector-quadrotor (built from the source) apparently [disables the motors in default](https://answers.ros.org/question/256590/hector-quadcopter-not-responding-to-cmd_vel-messages/) and it is required to call `enable_motors` service. The code in this repository basically calls this service internally, but in some case, here is how to call the service to enable the motors.
```
rosservice call /enable_motors true
```
The service name may be different and you should search it by `rosservice list | grep enable_motors`.

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

### How to run?

**At first, launch the gazebo by one of the launch files above.**
To run reactive agents for the signle UAV,
```
roslaunch quadrotor_tunnel_nav single_control.launch
```
To run reactive agents for the two UAVs,
```
roslaunch quadrotor_tunnel_nav multi_control.launch
```

## SLAM

### OctoMap only

This is just for practice purpose of [Octomap](http://wiki.ros.org/octomap).

In the terminal window, run the following to start Gazebo.
```
roslaunch quadrotor_tunnel_nav single_uav_IndianTunnel.launch
```

In another terminal window, this command will start mapping.
```
roslaunch quadrotor_tunnel_nav start_octomap.launch
```
Rviz will show the resulted map constructed by Octomap.

### MRPT

Note: this part is experimental.

[Mobile Robot Programming Toolkit (MRPT)](http://wiki.ros.org/mrpt_slam) is a set of libraries for robot navigation.
It implements Rao-Blackwellized particle filter for SLAM with Octomap.

This work is based on the source code of [RBPF-SLAM](https://www.mrpt.org/list-of-mrpt-apps/application-rbpf-slam/).
Customized Octomap with different likelihood evaluation and other features are added for the quadrotor navigation. (it is not so successful)

In the terminal window, this command will start Gazebo with a quadrotor which is
controlled based on the pose estimation, not the ground truth location.
```
roslaunch quadrotor_tunnel_nav start_env.launch
```
Note: The pose estimation is too unstable at the beginning.
You should wait for 30 seconds before running the following command.

In another window,
```
roslaunch quadrotor_tunnel_nav rbpf_slam.launch
```

Details:
TBA

---

# Setup

This package should run in the following environments.

1. Ubuntu18.04 (Bionic)
    - ROS: Melodic
    - Gazebo: Gazebo9

## Old version
The old version `tag: gazebo7` runs with Ubuntu16.04 (Xenial), ROS Kinetic, and Gazebo7.
Checkout the tagged commit `gezbo7` and see the README.md.
To clone the old version of this repository,
```
git clone --branch gazebo7 git@github.com:tidota/quadrotor-tunnel-nav.git
```

## Installation
If you do not have wstool,
```
sudo apt-get install python-wstool
```

Assume you are in the catkin workspace.

Make a `src` directory if you do not have it yet.
```
mkdir src
```

In the `src`, make or download [quadrotor_tunnel_nav.rosinstall](https://raw.githubusercontent.com/tidota/quadrotor-tunnel-nav/master/quadrotor_tunnel_nav.rosinstall) containing the following
```
- git: {local-name: hector_quadrotor, uri: 'https://github.com/tu-darmstadt-ros-pkg/hector_quadrotor.git', version: kinetic-devel}
- git: {local-name: hector_localization, uri: 'https://github.com/tu-darmstadt-ros-pkg/hector_localization.git', version: catkin}
- git: {local-name: hector_gazebo, uri: 'https://github.com/tu-darmstadt-ros-pkg/hector_gazebo.git', version: kinetic-devel}
- git: {local-name: hector_models, uri: 'https://github.com/tu-darmstadt-ros-pkg/hector_models.git', version: kinetic-devel}
- git: {local-name: hector_slam, uri: 'https://github.com/tu-darmstadt-ros-pkg/hector_slam.git', version: catkin}
- git: {local-name: quadrotor-tunnel-nav, uri: 'https://github.com/tidota/quadrotor-tunnel-nav.git', version: master}
```

Finally, run the following commands.
```
wstool init src ./src/quadrotor_tunnel_nav.rosinstall
wstool update -t src
rosdep install --from-paths src --ignore-src --rosdistro=melodic -y
```

For Octomap, you will also need `octomap_ros`
```
sudo apt install ros-melodic-octomap-ros
```

If you want to control the robot by the keyboard,
```
sudo apt-get install ros-melodic-teleop-twist-keyboard
```

---

# Ad Hoc Network

![environment for ad hoc network simulation](img/adhoc_env.jpg)

This package also simulates Ad Hoc network composed of multiple UAVs based on [AllNet](http://alnt.org/). Please note that it will require high machine power. A desktop PC or workstation is recommended.
This part of the package was tested and used on HP Z620.

In a rectangular space, 10 UAVs fly around by the similar reactive controller mentioned above. The configuration of robot formation and network is defined in `config/adhoc/robots.yaml` and `config/adhoc/simconfig.yaml`.

The communication is managed by a world plugin `AdHocNetPlugin` and a model plugin `AdHocClientPlugin`. The model plugin is applied to each robot and communicates with the world plugin. The world plugin checks the robot's location and determines to pass a packet based on the communication range.

As of October 10, 2018, they are just broadcasting packets, but epidemic routing will be implemented in the future.

---
Before running the simulation, the following command must run so that multiple nodes can open sockets to communicate with each other.
```
ulimit -n 9000
```

If this does not work, [this page](https://superuser.com/questions/1200539/cannot-increase-open-file-limit-past-4096-ubuntu) may work.
In `/etc/systemd/user.conf` and `/etc/systemd/system.conf`, edit the line of `#DefaultLimitNOFILE=` like this
```
DefaultLimitNOFILE=9000
```
And in `/etc/security/limits.conf`, add the following lines
```
<Your user ID> hard nofile 9000
<Your user ID> soft nofile 9000
```
---

Then, in your catkin workspace, build the program and run it as follows.

```
. /opt/ros/kinetic/setup.bash
catkin_make
. devel/setup.bash
roslaunch quadrotor_tunnel_nav I-tunnel-40x10.launch
```

The results are stored in csv files under `~/.ros/`

When you restart the simulation more than once, you had better to remove old
results, including ROS's log files.
```
rm ~/.ros/*.csv
rosclean purge
```

---

# Memorandum
[Misc notes of ROS commands](MEMORANDUM.md)
