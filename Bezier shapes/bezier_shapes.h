#ifndef BEZIER_SHAPES_H
#define BEZIER_SHAPES_H

#include <vector>
#include <cmath>        // For M_PI, sqrt, cos, sin, atan2, std::pow, std::fmod, std::abs
#include <string>       // For kwarg-like struct (optional) - though not used with current approach
#include <numeric>      // For std::iota, std::accumulate
#include <algorithm>    // For std::sort, std::transform, std::generate_n
#include <stdexcept>    // For std::runtime_error
#include <iostream>     // For debugging, if needed (can be removed if no debug output from header)
#include <random>       // For std::mt19937, std::random_device, std::uniform_real_distribution

// Define M_PI if not already defined (common on some compilers)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Structure to represent a 2D point
struct Point {
    double x, y;

    Point(double x_ = 0.0, double y_ = 0.0) : x(x_), y(y_) {}

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    Point operator*(double scalar) const {
        return Point(x * scalar, y * scalar);
    }
    double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }
    double magSq() const {
        return x * x + y * y;
    }
    double mag() const {
        return std::sqrt(magSq());
    }
};

// --- Helper: linspace ---
inline std::vector<double> linspace(double start, double end, int num) {
    std::vector<double> linspaced;
    if (num == 0) { return linspaced; }
    if (num == 1) {
        linspaced.push_back(start);
        return linspaced;
    }
    double delta = (end - start) / (num - 1);
    for (int i = 0; i < num; ++i) {
        linspaced.push_back(start + delta * i);
    }
    return linspaced;
}

// --- Binomial Coefficient ---
inline long long binom(int n, int k) {
    if (k < 0 || k > n) {
        return 0;
    }
    if (k == 0 || k == n) {
        return 1;
    }
    if (k > n / 2) {
        k = n - k;
    }
    long long res = 1;
    for (int i = 1; i <= k; ++i) {
        res = res * (n - i + 1) / i;
    }
    return res;
}

// --- Bernstein Polynomial ---
inline double bernstein(int n, int k, double t) {
    return static_cast<double>(binom(n, k)) * std::pow(t, k) * std::pow(1.0 - t, n - k);
}

// --- Bezier Curve ---
inline std::vector<Point> bezier(const std::vector<Point>& control_pts, int num = 200) {
    int N = control_pts.size();
    if (N == 0) return {};

    std::vector<double> t_values = linspace(0.0, 1.0, num);
    std::vector<Point> curve(num, Point(0.0, 0.0));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < num; ++j) {
            double bern = bernstein(N - 1, i, t_values[j]);
            curve[j].x += bern * control_pts[i].x;
            curve[j].y += bern * control_pts[i].y;
        }
    }
    return curve;
}

// Class representing a segment of the curve (cubic Bezier)
class Segment {
public:
    Point p1, p2;
    double angle1, angle2;
    int numpoints = 100;    // In-class initializer
    double r_factor = 0.3;  // In-class initializer
    double r_dist;

    std::vector<Point> control_points;
    std::vector<Point> curve_points;

    Segment(Point p1_, Point p2_, double angle1_, double angle2_, 
            int numpoints_val = 100, double r_val = 0.3)
        : p1(p1_), p2(p2_), angle1(angle1_), angle2(angle2_), 
          numpoints(numpoints_val), r_factor(r_val) {
        
        Point diff = p2 - p1;
        double d_dist = diff.mag();
        this->r_dist = this->r_factor * d_dist;

        control_points.resize(4);
        control_points[0] = this->p1;
        control_points[3] = this->p2;
        
        calc_intermediate_points();
        
        curve_points = bezier(this->control_points, this->numpoints);
    }
    
private:
    void calc_intermediate_points() {
        control_points[1] = p1 + Point(r_dist * std::cos(angle1), r_dist * std::sin(angle1));
        control_points[2] = p2 + Point(r_dist * std::cos(angle2 + M_PI), r_dist * std::sin(angle2 + M_PI));
    }
};

// Function to get the full curve by connecting segments
struct PointWithAngle {
    Point p;
    double angle;
};

inline std::pair<std::vector<Segment>, std::vector<Point>> get_curve(
    const std::vector<PointWithAngle>& points_with_angles, 
    int numpoints_val = 100, double r_val = 0.3) {
    
    std::vector<Segment> segments;
    if (points_with_angles.size() < 2) {
        return {segments, {}};
    }

    for (size_t i = 0; i < points_with_angles.size() - 1; ++i) {
        segments.emplace_back(
            points_with_angles[i].p,
            points_with_angles[i+1].p,
            points_with_angles[i].angle,
            points_with_angles[i+1].angle,
            numpoints_val,
            r_val
        );
    }

    std::vector<Point> full_curve_points;
    for (const auto& seg : segments) {
        full_curve_points.insert(full_curve_points.end(), seg.curve_points.begin(), seg.curve_points.end());
    }
    return {segments, full_curve_points};
}

// Function to sort points counter-clockwise
inline std::vector<Point> ccw_sort(std::vector<Point> p_vec) {
    if (p_vec.empty()) return {};

    Point centroid(0,0);
    for(const auto& pt : p_vec) {
        centroid.x += pt.x;
        centroid.y += pt.y;
    }
    centroid.x /= p_vec.size();
    centroid.y /= p_vec.size();

    std::sort(p_vec.begin(), p_vec.end(), [&](const Point& a, const Point& b) {
        double angle_a = std::atan2(a.y - centroid.y, a.x - centroid.x);
        double angle_b = std::atan2(b.y - centroid.y, b.x - centroid.x);
        return angle_a < angle_b;
    });
    return p_vec;
}

// Helper to normalize angle to [0, 2*PI)
inline double normalize_angle_positive(double angle) {
    angle = std::fmod(angle, 2.0 * M_PI);
    if (angle < 0) {
        angle += 2.0 * M_PI;
    }
    return angle;
}

// Function to get the final Bezier curve through a set of points 'a'
struct BezierCurveResult {
    std::vector<double> x_coords;
    std::vector<double> y_coords;
    std::vector<PointWithAngle> augmented_points;
};

inline BezierCurveResult get_bezier_curve(std::vector<Point> a_pts, double rad = 0.2, double edgy = 0.0) {
    if (a_pts.empty()) return {};

    double p_edgy_factor = std::atan(edgy) / M_PI + 0.5;

    a_pts = ccw_sort(a_pts);
    
    std::vector<PointWithAngle> a_with_angles;
    a_with_angles.reserve(a_pts.size() + 1); 
    for(const auto& pt : a_pts) {
        a_with_angles.push_back({pt, 0.0});
    }
    if (!a_pts.empty()) {
        a_with_angles.push_back({a_pts[0], 0.0});
    }
    
    if (a_with_angles.size() < 2) return {};

    std::vector<double> segment_angles;
    for (size_t i = 0; i < a_with_angles.size() - 1; ++i) {
        Point diff = a_with_angles[i+1].p - a_with_angles[i].p;
        segment_angles.push_back(normalize_angle_positive(std::atan2(diff.y, diff.x)));
    }
    
    if (segment_angles.empty()) return {};

    std::vector<double> tangent_angles_at_vertices;
    tangent_angles_at_vertices.resize(segment_angles.size());

    for (size_t i = 0; i < segment_angles.size(); ++i) {
        double ang1 = segment_angles[i];
        double ang2 = (i == 0) ? segment_angles.back() : segment_angles[i-1];
        
        double tangent = p_edgy_factor * ang1 + (1.0 - p_edgy_factor) * ang2;
        if (std::abs(ang2 - ang1) > M_PI) {
            tangent += M_PI;
        }
        tangent_angles_at_vertices[i] = normalize_angle_positive(tangent);
    }

    for(size_t i=0; i < tangent_angles_at_vertices.size(); ++i) {
        a_with_angles[i].angle = tangent_angles_at_vertices[i];
    }
    if (!tangent_angles_at_vertices.empty()) {
         a_with_angles.back().angle = tangent_angles_at_vertices[0];
    }

    auto curve_data = get_curve(a_with_angles, 100, rad);
    
    BezierCurveResult result;
    result.augmented_points = a_with_angles;
    for(const auto& pt : curve_data.second) {
        result.x_coords.push_back(pt.x);
        result.y_coords.push_back(pt.y);
    }
    
    return result;
}

// --- Random Number Generation Helpers ---
inline std::mt19937& get_random_engine() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

inline double random_double(double min_val = 0.0, double max_val = 1.0) {
    std::uniform_real_distribution<> dis(min_val, max_val);
    return dis(get_random_engine());
}

// Function to generate random points
inline std::vector<Point> get_random_points(
    int n = 5, 
    double scale = 0.8, 
    double mindst_param = -1.0,
    int rec = 0) {
    double min_dist_val = (mindst_param < 0.0) ? ( (n == 0 ? 0.7 : 0.7 / n) ) : mindst_param; // Avoid division by zero if n=0
    if (n == 0) return {};


    std::vector<Point> current_points(n);
    for (int i = 0; i < n; ++i) {
        current_points[i] = Point(random_double(), random_double());
    }

    if (n <= 1) {
        for(auto& pt : current_points) { pt = pt * scale; }
        return current_points;
    }
    
    std::vector<Point> sorted_points = ccw_sort(current_points);
    std::vector<double> distances;

    for (size_t i = 0; i < sorted_points.size() -1; ++i) {
        Point diff = sorted_points[i+1] - sorted_points[i];
        distances.push_back(std::abs(diff.x + diff.y)); // Original logic, consider if Point::mag() is more appropriate
    }
    if (sorted_points.size() > 1) { // Distance between last and first
         Point diff = sorted_points.front() - sorted_points.back();
         distances.push_back(std::abs(diff.x + diff.y)); // Original logic
    }

    bool all_distances_ok = true;
    if (distances.empty() && n > 1) all_distances_ok = false;

    for (double d_val : distances) {
        if (d_val < min_dist_val) {
            all_distances_ok = false;
            break;
        }
    }

    if (all_distances_ok || rec >= 200) {
        for (auto& pt : current_points) {
            pt = pt * scale;
        }
        return current_points;
    } else {
        return get_random_points(n, scale, min_dist_val, rec + 1);
    }
}

#endif // BEZIER_SHAPES_H
