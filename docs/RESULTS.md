# Validation Results

## Environment

- Ubuntu environment with ROS2 Jazzy.
- C++ compiler: GNU 13.3.0.
- Build tool: `colcon`.
- Visualization target: RViz2 configuration included in the package.

## Automated checks

```text
ROS2 package build: passed
Python geometry tests: 2 passed
ROS2 test summary: 3 tests, 0 errors, 0 failures, 0 skipped
```

## Runtime checks

The launch file was run with RViz disabled for headless validation:

```text
/points: active
/points_transformed: active
transformed message frame_id: lidar_frame
transformed message width: 352 points
configured rotation: 30 degrees around Z
```

## Interpretation

The pipeline successfully creates a synthetic room-and-obstacle scene, publishes it as `sensor_msgs/PointCloud2`, rotates the XY coordinates in the C++ node, and publishes the transformed cloud. This is software-in-the-loop validation only; it is not physical LiDAR validation.

## Evidence to add later

- RViz screenshot with input and transformed clouds visible.
- Short screen recording of the launch and topic inspection.
- Real point-cloud replay if a permitted dataset becomes available.
