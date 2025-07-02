#include "bezier_shapes.h"
#include <numeric>      // For std::iota, std::accumulate
#include <algorithm>    // For std::sort, std::transform, std::generate_n
#include <vector>
#include <cmath>
#include <stdexcept>    // For std::runtime_error
#include <iostream>     // For debugging, if needed

// --- Helper: linspace ---
std::vector<double> linspace(double start, double end, int num) {
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
// A simple implementation. For larger n, k, a more robust version (e.g., using logs) might be needed.
long long binom(int n, int k) {
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
double bernstein(int n, int k, double t) {
    return static_cast<double>(binom(n, k)) * std::pow(t, k) * std::pow(1.0 - t, n - k);
}

// --- Bezier Curve ---
std::vector<Point> bezier(const std::vector<Point>& control_pts, int num) {
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

// --- Segment Class Implementation ---
Segment::Segment(Point p1_, Point p2_, double angle1_, double angle2_, 
                 int numpoints_val, double r_val)
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

void Segment::calc_intermediate_points() {
    control_points[1] = p1 + Point(r_dist * std::cos(angle1), r_dist * sin(angle1));
    control_points[2] = p2 + Point(r_dist * std::cos(angle2 + M_PI), r_dist * sin(angle2 + M_PI));
}

// --- get_curve Implementation ---
std::pair<std::vector<Segment>, std::vector<Point>> get_curve(
    const std::vector<PointWithAngle>& points_with_angles, 
    int numpoints_val, double r_val) {
    
    std::vector<Segment> segments;
    if (points_with_angles.size() < 2) {
        // Not enough points to form a segment
        return {segments, {}};
    }

    for (size_t i = 0; i < points_with_angles.size() - 1; ++i) {
        segments.emplace_back(
            points_with_angles[i].p,
            points_with_angles[i+1].p,
            points_with_angles[i].angle,
            points_with_angles[i+1].angle,
            numpoints_val, // Pass numpoints_val
            r_val          // Pass r_val
        );
    }

    std::vector<Point> full_curve_points;
    for (const auto& seg : segments) {
        full_curve_points.insert(full_curve_points.end(), seg.curve_points.begin(), seg.curve_points.end());
    }
    return {segments, full_curve_points};
}

// --- ccw_sort Implementation ---
std::vector<Point> ccw_sort(std::vector<Point> p_vec) {
    if (p_vec.empty()) return {};

    // Calculate centroid
    Point centroid(0,0);
    for(const auto& pt : p_vec) {
        centroid.x += pt.x;
        centroid.y += pt.y;
    }
    centroid.x /= p_vec.size();
    centroid.y /= p_vec.size();

    // Sort by angle around centroid
    std::sort(p_vec.begin(), p_vec.end(), [&](const Point& a, const Point& b) {
        double angle_a = std::atan2(a.y - centroid.y, a.x - centroid.x); // Standard atan2(y,x)
        double angle_b = std::atan2(b.y - centroid.y, b.x - centroid.x); // Standard atan2(y,x)
        // The Python version uses atan2(d[:,0], d[:,1]), which is atan2(x,y).
        // To match that behavior for sorting:
        // angle_a = std::atan2(a.x - centroid.x, a.y - centroid.y);
        // angle_b = std::atan2(b.x - centroid.x, b.y - centroid.y);
        return angle_a < angle_b;
    });
    return p_vec;
}

// Helper to normalize angle to [0, 2*PI)
double normalize_angle_positive(double angle) {
    angle = std::fmod(angle, 2.0 * M_PI);
    if (angle < 0) {
        angle += 2.0 * M_PI;
    }
    return angle;
}


// --- get_bezier_curve Implementation ---
BezierCurveResult get_bezier_curve(std::vector<Point> a_pts, double rad, double edgy) {
    if (a_pts.empty()) return {};

    double p_edgy_factor = std::atan(edgy) / M_PI + 0.5;

    a_pts = ccw_sort(a_pts);
    
    std::vector<PointWithAngle> a_with_angles; // This will store points with their calculated tangent angles
    // Reserve space for points + appended first point + angles
    a_with_angles.reserve(a_pts.size() + 1); 
    for(const auto& pt : a_pts) {
        a_with_angles.push_back({pt, 0.0}); // Initialize angle to 0.0
    }
    if (!a_pts.empty()) { // Close the loop
        a_with_angles.push_back({a_pts[0], 0.0});
    }
    
    if (a_with_angles.size() < 2) return {}; // Not enough points after closing

    std::vector<double> segment_angles; // 'ang' in Python
    for (size_t i = 0; i < a_with_angles.size() - 1; ++i) {
        Point diff = a_with_angles[i+1].p - a_with_angles[i].p;
        segment_angles.push_back(normalize_angle_positive(std::atan2(diff.y, diff.x)));
    }
    
    if (segment_angles.empty()) return {};


    std::vector<double> tangent_angles_at_vertices; // 'ang' after averaging
    tangent_angles_at_vertices.resize(segment_angles.size());

    for (size_t i = 0; i < segment_angles.size(); ++i) {
        double ang1 = segment_angles[i]; // Angle of segment leaving current vertex
        double ang2 = (i == 0) ? segment_angles.back() : segment_angles[i-1]; // Angle of segment entering current vertex (rolled)
        
        double tangent = p_edgy_factor * ang1 + (1.0 - p_edgy_factor) * ang2;
        if (std::abs(ang2 - ang1) > M_PI) {
            tangent += M_PI; // Correction for wrap-around
        }
        tangent_angles_at_vertices[i] = normalize_angle_positive(tangent);
    }

    // Assign calculated tangent angles to the points in a_with_angles
    // The last point in a_with_angles is a_pts[0], its tangent should be the first calculated tangent
    for(size_t i=0; i < tangent_angles_at_vertices.size(); ++i) {
        a_with_angles[i].angle = tangent_angles_at_vertices[i];
    }
    // The angle for the duplicated first point (now at the end) should match the first point's angle
    if (!tangent_angles_at_vertices.empty()) {
         a_with_angles.back().angle = tangent_angles_at_vertices[0];
    }


    // Pass rad for r_val, and default 100 for numpoints_val to get_curve
    auto curve_data = get_curve(a_with_angles, 100, rad);
    
    BezierCurveResult result;
    result.augmented_points = a_with_angles;
    for(const auto& pt : curve_data.second) {
        result.x_coords.push_back(pt.x);
        result.y_coords.push_back(pt.y);
    }
    
    return result;
}


// --- get_random_points Implementation ---
// Note: C++ random number generation is more verbose than numpy.random.rand
#include <random>
std::mt19937& get_random_engine() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

double random_double(double min_val = 0.0, double max_val = 1.0) {
    std::uniform_real_distribution<> dis(min_val, max_val);
    return dis(get_random_engine());
}

std::vector<Point> get_random_points(int n, double scale, double mindst_param, int rec) {
    double min_dist_val = (mindst_param < 0.0) ? (0.7 / n) : mindst_param;

    std::vector<Point> current_points(n);
    for (int i = 0; i < n; ++i) {
        current_points[i] = Point(random_double(), random_double());
    }

    if (n <= 1) { // No distances to check for 0 or 1 point
        for(auto& pt : current_points) { pt = pt * scale; }
        return current_points;
    }
    
    std::vector<Point> sorted_points = ccw_sort(current_points);
    std::vector<double> distances;

    // The Python code's distance: d = np.sqrt(np.sum(np.diff(ccw_sort(a), axis=0), axis=1)**2)
    // This is sqrt((dx+dy)^2) = |dx+dy|
    for (size_t i = 0; i < sorted_points.size() -1; ++i) { // Distances between p[i] and p[i+1]
        Point diff = sorted_points[i+1] - sorted_points[i];
        distances.push_back(std::abs(diff.x + diff.y));
    }
    // And distance between last and first point to close the loop for checking
    if (sorted_points.size() > 1) {
         Point diff = sorted_points.front() - sorted_points.back();
         distances.push_back(std::abs(diff.x + diff.y));
    }


    bool all_distances_ok = true;
    if (distances.empty() && n > 1) all_distances_ok = false; // Should have distances if n > 1

    for (double d_val : distances) {
        if (d_val < min_dist_val) {
            all_distances_ok = false;
            break;
        }
    }

    if (all_distances_ok || rec >= 200) {
        for (auto& pt : current_points) { // Scale the original (unsorted) points
            pt = pt * scale;
        }
        return current_points;
    } else {
        return get_random_points(n, scale, min_dist_val, rec + 1);
    }
}
