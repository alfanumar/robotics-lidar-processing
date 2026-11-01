cmake_minimum_required(VERSION 3.8)
project(lidar_transform)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_pytest REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(lidar_transform_node src/lidar_transform_node.cpp)
ament_target_dependencies(lidar_transform_node rclcpp sensor_msgs)
target_compile_options(lidar_transform_node PRIVATE -Wall -Wextra -Wpedantic)

add_executable(simulated_lidar_publisher src/simulated_lidar_publisher.cpp)
ament_target_dependencies(simulated_lidar_publisher rclcpp sensor_msgs)
target_compile_options(simulated_lidar_publisher PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS lidar_transform_node simulated_lidar_publisher
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch rviz config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(launch_testing_ament_cmake REQUIRED)
  ament_add_pytest_test(test_geometry test/test_geometry.py)
  add_launch_test(test/test_pipeline_launch.py TIMEOUT 30)
endif()

ament_package()
