#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/point_cloud2_iterator.hpp"

class LidarTransformNode final : public rclcpp::Node {
public:
    LidarTransformNode() : Node("lidar_transform_node") {
        const double angle_degrees = declare_parameter<double>("angle_deg", 30.0);
        angle_radians_ = angle_degrees * std::acos(-1.0) / 180.0;
        cosine_ = std::cos(angle_radians_);
        sine_ = std::sin(angle_radians_);

        publisher_ = create_publisher<sensor_msgs::msg::PointCloud2>(
            "points_transformed", rclcpp::SensorDataQoS());
        subscription_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "points", rclcpp::SensorDataQoS(),
            [this](const sensor_msgs::msg::PointCloud2::SharedPtr message) {
                transform_and_publish(*message);
            });
    }

private:
    void transform_and_publish(const sensor_msgs::msg::PointCloud2& input) {
        sensor_msgs::msg::PointCloud2 output = input;
        sensor_msgs::PointCloud2ConstIterator<float> input_x(input, "x");
        sensor_msgs::PointCloud2ConstIterator<float> input_y(input, "y");
        sensor_msgs::PointCloud2Iterator<float> output_x(output, "x");
        sensor_msgs::PointCloud2Iterator<float> output_y(output, "y");

        for (std::size_t index = 0; index < input.width * input.height;
             ++index, ++input_x, ++input_y, ++output_x, ++output_y) {
            const float x = *input_x;
            const float y = *input_y;
            *output_x = static_cast<float>(cosine_ * x - sine_ * y);
            *output_y = static_cast<float>(sine_ * x + cosine_ * y);
        }
        publisher_->publish(output);
    }

    double angle_radians_{};
    double cosine_{};
    double sine_{};
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LidarTransformNode>());
    rclcpp::shutdown();
    return 0;
}
