#include "bezier_shapes.h"
#include <iostream>
#include <vector>
#include <iomanip> // For std::fixed, std::setprecision

// Helper to print a curve
void print_curve(const BezierCurveResult& curve_result, int curve_index) {
    std::cout << "Curve " << curve_index << ":" << std::endl;
    if (curve_result.x_coords.size() != curve_result.y_coords.size()) {
        std::cerr << "Error: X and Y coordinate counts differ for curve " << curve_index << std::endl;
        return;
    }
    for (size_t i = 0; i < curve_result.x_coords.size(); ++i) {
        std::cout << std::fixed << std::setprecision(4) 
                  << curve_result.x_coords[i] << " " 
                  << curve_result.y_coords[i] << std::endl;
    }
    std::cout << std::endl; // Separator for multiple curves
}


int main() {
    // Parameters from the Python example
    double rad = 0.2;
    double edgy = 0.05;
    int num_random_points = 7;
    double scale_random_points = 1.0;

    std::vector<Point> offsets = {
        Point(0,0), Point(0,1), Point(1,0), Point(1,1)
    };

    int curve_counter = 1;

    // --- First set of plots (equivalent to first fig, ax block) ---
    std::cout << "--- Generating first set of shapes ---" << std::endl;
    for (const auto& c_offset : offsets) {
        std::vector<Point> random_pts_raw = get_random_points(num_random_points, scale_random_points);
        std::vector<Point> a_pts;
        for (const auto& rp : random_pts_raw) {
            a_pts.push_back(rp + c_offset);
        }

        BezierCurveResult curve = get_bezier_curve(a_pts, rad, edgy);
        print_curve(curve, curve_counter++);
    }
    
    // --- Second set of plots (equivalent to second fig, ax block) ---
    // The Python code creates a new figure. Here we just generate more curves.
    // If you want to distinguish them, you could print to separate files
    // or add more metadata to the console output.
    std::cout << "\n--- Generating second set of shapes ---" << std::endl;
    curve_counter = 1; // Reset counter for the "new figure"
     for (const auto& c_offset : offsets) {
        std::vector<Point> random_pts_raw = get_random_points(num_random_points, scale_random_points);
        std::vector<Point> a_pts;
        for (const auto& rp : random_pts_raw) {
            a_pts.push_back(rp + c_offset);
        }

        BezierCurveResult curve = get_bezier_curve(a_pts, rad, edgy);
        print_curve(curve, curve_counter++);
    }


    std::cout << "\nTo visualize these points, copy the output for each curve into a plotting tool." << std::endl;
    std::cout << "For example, save each 'Curve X' block to a .dat file and use gnuplot:" << std::endl;
    std::cout << "  plot 'curve1.dat' with lines title 'Shape 1', \\" << std::endl;
    std::cout << "       'curve2.dat' with lines title 'Shape 2', \\" << std::endl;
    std::cout << "       ..." << std::endl;


    return 0;
}

// How to compile (example with clang++):
//
// IMPORTANT: The linker errors ("Undefined symbols") mean that bezier_shapes.cpp
// is NOT being compiled and linked with main.cpp.
// Your build command or IDE build task (e.g., VS Code's tasks.json)
// MUST include all .cpp source files.
//
// Example command for manual compilation in the terminal:
// clang++ -std=c++17 \
//         '/Users/jethroaiyesan/Programming/Tutorials/C++/Bezier shapes/main.cpp' \
//         '/Users/jethroaiyesan/Programming/Tutorials/C++/Bezier shapes/bezier_shapes.cpp' \
//         '/Users/jethroaiyesan/Programming/Tutorials/C++/includes/glad/src/glad.c' \
//         -o '/Users/jethroaiyesan/Programming/Tutorials/C++/Bezier shapes/bezier_generator' \
//         -I/opt/homebrew/include \
//         -I/Users/jethroaiyesan/Programming/Tutorials/C++/includes \
//         -L/opt/homebrew/lib -lglfw -lSDL2 -lSDL2_image -lSDL2_ttf \
//         -framework OpenGL -framework Cocoa -framework IOKit \
//         -Wall -Wextra
//
// If using an IDE's "build active file" feature, it might only compile the active file.
// You need to configure a build task that compiles all project source files.
//
// Then run:
// ./bezier_generator
// To save output to a file:
// ./bezier_generator > output_points.txt
