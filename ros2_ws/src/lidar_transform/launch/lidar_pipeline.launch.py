import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    package_share = get_package_share_directory("lidar_transform")
    parameters = os.path.join(package_share, "config", "pipeline.yaml")
    rviz_config = os.path.join(package_share, "rviz", "lidar_pipeline.rviz")

    return LaunchDescription([
        DeclareLaunchArgument("use_rviz", default_value="false"),
        Node(
            package="lidar_transform",
            executable="simulated_lidar_publisher",
            name="simulated_lidar_publisher",
            output="screen",
            parameters=[parameters],
        ),
        Node(
            package="lidar_transform",
            executable="lidar_transform_node",
            name="lidar_transform_node",
            output="screen",
            parameters=[parameters],
        ),
        Node(
            package="rviz2",
            executable="rviz2",
            name="rviz2",
            arguments=["-d", rviz_config],
            condition=IfCondition(LaunchConfiguration("use_rviz")),
            output="screen",
        ),
    ])
