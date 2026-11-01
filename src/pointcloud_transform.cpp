#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Point {
    double x{};
    double y{};
    double z{};
};

bool parse_point(std::string line, Point& point) {
    if (line.empty() || line.front() == '#') {
        return false;
    }
    for (char& character : line) {
        if (character == ',') {
            character = ' ';
        }
    }
    std::istringstream stream(line);
    return static_cast<bool>(stream >> point.x >> point.y >> point.z);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: pointcloud_transform INPUT.xyz OUTPUT.csv ANGLE_DEG\n";
        return 2;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "Cannot open input file: " << argv[1] << '\n';
        return 1;
    }

    std::vector<Point> points;
    std::string line;
    while (std::getline(input, line)) {
        Point point;
        if (parse_point(line, point)) {
            points.push_back(point);
        }
    }
    if (points.empty()) {
        std::cerr << "No XYZ points found in input.\n";
        return 1;
    }

    const double pi = std::acos(-1.0);
    const double angle = std::stod(argv[3]) * pi / 180.0;
    const double cosine = std::cos(angle);
    const double sine = std::sin(angle);

    std::ofstream output(argv[2]);
    if (!output) {
        std::cerr << "Cannot open output file: " << argv[2] << '\n';
        return 1;
    }
    output << std::fixed << std::setprecision(6) << "x,y,z\n";

    Point minimum{std::numeric_limits<double>::infinity(),
                  std::numeric_limits<double>::infinity(),
                  std::numeric_limits<double>::infinity()};
    Point maximum{-std::numeric_limits<double>::infinity(),
                  -std::numeric_limits<double>::infinity(),
                  -std::numeric_limits<double>::infinity()};

    for (const Point& point : points) {
        const Point rotated{cosine * point.x - sine * point.y,
                            sine * point.x + cosine * point.y,
                            point.z};
        output << rotated.x << ',' << rotated.y << ',' << rotated.z << '\n';
        minimum.x = std::min(minimum.x, rotated.x);
        minimum.y = std::min(minimum.y, rotated.y);
        minimum.z = std::min(minimum.z, rotated.z);
        maximum.x = std::max(maximum.x, rotated.x);
        maximum.y = std::max(maximum.y, rotated.y);
        maximum.z = std::max(maximum.z, rotated.z);
    }

    std::cout << "Processed " << points.size() << " points\n"
              << "Bounds after Z rotation: [" << minimum.x << ", " << maximum.x
              << "] x [" << minimum.y << ", " << maximum.y << "] x ["
              << minimum.z << ", " << maximum.z << "]\n";
    return 0;
}
