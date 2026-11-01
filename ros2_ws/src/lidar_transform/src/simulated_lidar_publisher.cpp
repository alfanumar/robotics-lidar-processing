#include <chrono>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/point_cloud2_iterator.hpp"

struct SimulatedPoint {
    float x;
    float y;
    float z;
};

class SimulatedLidarPublisher final : public rclcpp::Node {
public:
    SimulatedLidarPublisher() : Node("simulated_lidar_publisher") {
        frame_id_ = declare_parameter<std::string>("frame_id", "lidar_frame");
        const double publish_rate_hz = declare_parameter<double>("publish_rate_hz", 5.0);
        if (frame_id_.empty()) {
            throw std::invalid_argument("frame_id must not be empty");
        }
        if (!std::isfinite(publish_rate_hz) || publish_rate_hz <= 0.0) {
            throw std::invalid_argument("publish_rate_hz must be finite and greater than zero");
        }
        const auto period = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::duration<double>(1.0 / publish_rate_hz));

        publisher_ = create_publisher<sensor_msgs::msg::PointCloud2>(
            "points", rclcpp::SensorDataQoS());
        timer_ = create_wall_timer(period, [this]() { publish_scan(); });
        RCLCPP_INFO(
            get_logger(),
            "Publishing synthetic point cloud: frame_id='%s', rate=%.2f Hz, points=%zu",
            frame_id_.c_str(), publish_rate_hz, make_scene().size());
    }

private:
    static std::vector<SimulatedPoint> make_scene() {
        std::vector<SimulatedPoint> points;
        constexpr float wall_min = -4.0F;
        constexpr float wall_max = 4.0F;
        constexpr float step = 0.20F;

        // A square room boundary, representing returns from static walls.
        for (float value = wall_min; value <= wall_max; value += step) {
            points.push_back({value, wall_max, 0.0F});
            points.push_back({value, wall_max, 0.6F});
            points.push_back({value, wall_min, 0.0F});
            points.push_back({value, wall_min, 0.6F});
            points.push_back({wall_min, value, 0.0F});
            points.push_back({wall_min, value, 0.6F});
            points.push_back({wall_max, value, 0.0F});
            points.push_back({wall_max, value, 0.6F});
        }

        // A rectangular obstacle inside the room.
        constexpr float obstacle_min = -1.0F;
        constexpr float obstacle_max = 1.0F;
        constexpr float obstacle_y = 1.4F;
        for (float value = obstacle_min; value <= obstacle_max; value += step) {
            points.push_back({value, obstacle_y, 0.0F});
            points.push_back({value, obstacle_y, 0.8F});
        }
        for (float value = 0.0F; value <= 0.8F; value += 0.2F) {
            points.push_back({obstacle_min, obstacle_y, value});
            points.push_back({obstacle_max, obstacle_y, value});
        }
        return points;
    }

    void publish_scan() {
        static const auto points = make_scene();
        sensor_msgs::msg::PointCloud2 cloud;
        cloud.header.stamp = now();
        cloud.header.frame_id = frame_id_;

        sensor_msgs::PointCloud2Modifier modifier(cloud);
        modifier.setPointCloud2FieldsByString(1, "xyz");
        modifier.resize(points.size());

        sensor_msgs::PointCloud2Iterator<float> x_iterator(cloud, "x");
        sensor_msgs::PointCloud2Iterator<float> y_iterator(cloud, "y");
        sensor_msgs::PointCloud2Iterator<float> z_iterator(cloud, "z");
        for (const auto& point : points) {
            *x_iterator = point.x;
            *y_iterator = point.y;
            *z_iterator = point.z;
            ++x_iterator;
            ++y_iterator;
            ++z_iterator;
        }
        publisher_->publish(cloud);
    }

    std::string frame_id_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SimulatedLidarPublisher>());
    rclcpp::shutdown();
    return 0;
}
