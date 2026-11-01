import math
import time
import unittest

import launch
import launch_ros.actions
import launch_testing.actions
import pytest
import rclpy
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import PointCloud2
from sensor_msgs_py import point_cloud2


TEST_NAMESPACE = "pointcloud_pipeline_test"
ANGLE_DEGREES = 30.0


@pytest.mark.rostest
def generate_test_description():
    publisher = launch_ros.actions.Node(
        package="lidar_transform",
        executable="simulated_lidar_publisher",
        namespace=TEST_NAMESPACE,
        name="simulated_lidar_publisher",
        parameters=[{"frame_id": "lidar_frame", "publish_rate_hz": 5.0}],
        output="screen",
    )
    transformer = launch_ros.actions.Node(
        package="lidar_transform",
        executable="lidar_transform_node",
        namespace=TEST_NAMESPACE,
        name="lidar_transform_node",
        parameters=[{"angle_deg": ANGLE_DEGREES}],
        output="screen",
    )

    return (
        launch.LaunchDescription(
            [publisher, transformer, launch_testing.actions.ReadyToTest()]
        ),
        {"publisher": publisher, "transformer": transformer},
    )


class TestPointCloudPipeline(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        self.node = rclpy.create_node("pointcloud_pipeline_assertions")

    def tearDown(self):
        self.node.destroy_node()

    def test_pointcloud_message_is_transformed(self):
        received = {"input": None, "output": None}

        input_subscription = self.node.create_subscription(
            PointCloud2,
            f"/{TEST_NAMESPACE}/points",
            lambda message: received.__setitem__("input", message),
            qos_profile_sensor_data,
        )
        output_subscription = self.node.create_subscription(
            PointCloud2,
            f"/{TEST_NAMESPACE}/points_transformed",
            lambda message: received.__setitem__("output", message),
            qos_profile_sensor_data,
        )

        try:
            deadline = time.monotonic() + 15.0
            while time.monotonic() < deadline:
                rclpy.spin_once(self.node, timeout_sec=0.1)
                if received["input"] is not None and received["output"] is not None:
                    break

            self.assertIsNotNone(received["input"], "Input PointCloud2 was not received")
            self.assertIsNotNone(
                received["output"], "Transformed PointCloud2 was not received"
            )

            input_message = received["input"]
            output_message = received["output"]
            input_points = point_cloud2.read_points_list(
                input_message, field_names=["x", "y", "z"], skip_nans=True
            )
            output_points = point_cloud2.read_points_list(
                output_message, field_names=["x", "y", "z"], skip_nans=True
            )

            self.assertEqual(input_message.header.frame_id, "lidar_frame")
            self.assertEqual(output_message.header.frame_id, "lidar_frame")
            self.assertEqual(len(input_points), 352)
            self.assertEqual(len(output_points), len(input_points))

            angle = math.radians(ANGLE_DEGREES)
            cosine = math.cos(angle)
            sine = math.sin(angle)
            for before, after in zip(input_points, output_points):
                expected_x = cosine * before.x - sine * before.y
                expected_y = sine * before.x + cosine * before.y
                self.assertAlmostEqual(after.x, expected_x, places=5)
                self.assertAlmostEqual(after.y, expected_y, places=5)
                self.assertAlmostEqual(after.z, before.z, places=5)
        finally:
            self.node.destroy_subscription(input_subscription)
            self.node.destroy_subscription(output_subscription)
