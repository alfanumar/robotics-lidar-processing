# ROS2 LiDAR Point-Cloud Processing — Simulation-Based Portfolio Project

This project demonstrates a complete simulation-based LiDAR processing pipeline. A simulated publisher creates a room and obstacle scene as `sensor_msgs/PointCloud2`, a C++ ROS2 node rotates the point cloud around the Z axis, and RViz can display the input and transformed outputs.

The project is a personal validation implementation. It does not claim physical LiDAR validation or reproduce the original laboratory source code. The included scene is synthetic and is clearly separated from any future real dataset.

## Architecture

```text
simulated_lidar_publisher
          | /points (PointCloud2)
          v
lidar_transform_node (C++ / ROS2)
          | /points_transformed (PointCloud2)
          v
       RViz2
```

The original CV project used ROS, RViz, and PCL. This demo must be described as a **personal validation demo** unless the original source code is recovered and published.

## Run

```bash
cmake -S . -B build
cmake --build build
./build/pointcloud_transform data/sample.xyz /tmp/rotated.csv 30
```

## ROS2 simulation

Requirements: Ubuntu with ROS2 Jazzy, `colcon`, and RViz2.

```bash
source /opt/ros/jazzy/setup.bash
cd ros2_ws
colcon build --packages-select lidar_transform
source install/setup.bash
ros2 launch lidar_transform lidar_pipeline.launch.py use_rviz:=false
```

To open RViz with the saved display configuration:

```bash
ros2 launch lidar_transform lidar_pipeline.launch.py use_rviz:=true
```

Useful inspection commands in another terminal:

```bash
source /opt/ros/jazzy/setup.bash
source ros2_ws/install/setup.bash
ros2 topic list
ros2 topic hz /points
ros2 topic echo /points_transformed --once
```

The simulated publisher generates a static room boundary and a rectangular obstacle. The transformed cloud rotates the XY coordinates by 30 degrees while preserving the Z coordinate. Dataset policy and the reserved real-data location are documented in [`data/README.md`](data/README.md).

Current validation results are recorded in [`docs/RESULTS.md`](docs/RESULTS.md). Media and publication guidance are in [`media/README.md`](media/README.md). The current simulation evidence includes [`media/rviz-simulation.png`](media/rviz-simulation.png) and [`media/pipeline-diagram.svg`](media/pipeline-diagram.svg).

## Tests

```bash
source /opt/ros/jazzy/setup.bash
colcon test --packages-select lidar_transform \
  --base-paths ros2_ws --build-base /tmp/lidar-build \
  --install-base /tmp/lidar-install --log-base /tmp/lidar-test-log
colcon test-result --all --verbose
```

The tests verify the rotation geometry and distance preservation. Runtime validation should also confirm both ROS2 topics are active.

## Portfolio evidence to add

- Add a screenshot of RViz showing the two topics.
- Add a short screen recording of the launch and topic inspection.
- Record point count, publish rate, transformation angle, and runtime.
- Add a real point cloud only when it is available and permitted to publish.
- Keep synthetic and real datasets in separate folders with clear labels.
