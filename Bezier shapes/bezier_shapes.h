#ifndef BEZIER_SHAPES_H
#define BEZIER_SHAPES_H

#include <vector>
#include <cmath> // For M_PI, sqrt, cos, sin, atan2
#include <string> // For kwarg-like struct (optional)

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
    // Dot product (though not explicitly used in Python for Point, useful for geometry)
    double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }
    // Magnitude squared
    double magSq() const {
        return x * x + y * y;
    }
    // Magnitude
    double mag() const {
        return std::sqrt(magSq());
    }
};

// Function to calculate binomial coefficient C(n, k)
long long binom(int n, int k);

// Function to calculate Bernstein polynomial
double bernstein(int n, int k, double t);

// Function to generate points on a Bezier curve
std::vector<Point> bezier(const std::vector<Point>& points, int num = 200);

// Class representing a segment of the curve (cubic Bezier)
class Segment {
public:
    Point p1, p2;
    double angle1, angle2;
    int numpoints = 100;    // In-class initializer
    double r_factor = 0.3;  // In-class initializer
    double r_dist;   // Corresponds to 'self.r' in Python (r*d)

    std::vector<Point> control_points; // p[0] to p[3]
    std::vector<Point> curve_points;   // The generated curve for this segment

    Segment(Point p1_, Point p2_, double angle1_, double angle2_, 
            int numpoints_val = 100, double r_val = 0.3);
    
private:
    void calc_intermediate_points();
};

// Function to get the full curve by connecting segments
// The 'points' input in Python is [x, y, angle]. We'll use a struct or similar.
struct PointWithAngle {
    Point p;
    double angle;
};
std::pair<std::vector<Segment>, std::vector<Point>> get_curve(
    const std::vector<PointWithAngle>& points_with_angles, 
    int numpoints_val = 100, double r_val = 0.3);

// Function to sort points counter-clockwise
std::vector<Point> ccw_sort(std::vector<Point> p_vec);

// Function to get the final Bezier curve through a set of points 'a'
struct BezierCurveResult {
    std::vector<double> x_coords;
    std::vector<double> y_coords;
    std::vector<PointWithAngle> augmented_points; // Corresponds to 'a' in Python return
};
BezierCurveResult get_bezier_curve(
    std::vector<Point> a, 
    double rad = 0.2, 
    double edgy = 0.0);

// Function to generate random points
std::vector<Point> get_random_points(
    int n = 5, 
    double scale = 0.8, 
    double mindst_param = -1.0, // Use -1 or some indicator for 'None'
    int rec = 0);

#endif // BEZIER_SHAPES_H
