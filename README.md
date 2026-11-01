# ROS2 LiDAR Point-Cloud Processing — Simulation-Based Portfolio Project

[![ROS 2 Jazzy CI](https://github.com/alfanumar/robotics-lidar-processing/actions/workflows/ros2.yml/badge.svg)](https://github.com/alfanumar/robotics-lidar-processing/actions/workflows/ros2.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

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

## Demo evidence

- [RViz screenshot](media/rviz-simulation.png)
- [Pipeline diagram](media/pipeline-diagram.svg)
- [Screen recording](media/pipeline-demo.mp4)

The original CV project used ROS, RViz, and PCL. This repository is a personal ROS2 validation implementation and uses `sensor_msgs/PointCloud2` directly; it does not claim to reproduce the original source code or require PCL.

## Standalone C++ demo

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

The simulated publisher generates a static room boundary and a rectangular obstacle. The transformed cloud rotates the XY coordinates by 30 degrees while preserving the Z coordinate. Dataset provenance and limitations are documented in [`docs/DATA_PROVENANCE.md`](docs/DATA_PROVENANCE.md).

Current validation results are recorded in [`docs/RESULTS.md`](docs/RESULTS.md). Media and publication guidance are in [`media/README.md`](media/README.md). The current simulation evidence includes [`media/rviz-simulation.png`](media/rviz-simulation.png) and [`media/pipeline-diagram.svg`](media/pipeline-diagram.svg).

## Tests

```bash
source /opt/ros/jazzy/setup.bash
cd ros2_ws
colcon test --packages-select lidar_transform
colcon test-result --all --verbose
```

The tests verify the rotation geometry, message count, frame ID, and the live C++ ROS2 transformation. Runtime validation confirms both ROS2 topics are active.

## Parameters and expected result

| Parameter | Value |
|---|---:|
| Frame ID | `lidar_frame` |
| Publish rate | `5 Hz` |
| Input points | `352` |
| Rotation | `30 degrees around Z` |
| Input topic | `/points` |
| Output topic | `/points_transformed` |

## Limitations

- The scene is synthetic and does not represent physical LiDAR measurements.
- The simulation does not model sensor noise, beam pattern, occlusion, intensity, or missing returns.
- Validation is software-in-the-loop; no hardware or field validation is claimed.
- The original CV project is not reproduced here; this is a personal validation implementation.

## Portfolio evidence

- RViz screenshot showing the two topics is included in `media/`.
- Screen recording of the launch and topic inspection is included in `media/`.
- Point count, publish rate, transformation angle, and runtime checks are recorded in [`docs/RESULTS.md`](docs/RESULTS.md).
- Synthetic and future real datasets are kept in separate folders with clear labels.
