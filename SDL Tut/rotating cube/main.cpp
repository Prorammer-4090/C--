#include "screen.h"
#include <numeric>



struct vec3{
    float x, y, z;
};

struct connection{
    int a, b;
};

void rotate(vec3& point, float x = 1, float y = 1, float z = 1){
    float rad = 0;
    float original_x, original_y, original_z;

    // Rotate around X-axis
    rad = x;
    original_y = point.y;
    original_z = point.z;
    point.y = std::cos(rad) * original_y - std::sin(rad) * original_z;
    point.z = std::sin(rad) * original_y + std::cos(rad) * original_z;

    // Rotate around Y-axis
    rad = y;
    original_x = point.x;
    original_z = point.z; // Use the point.z updated from X rotation
    point.x = std::cos(rad) * original_x + std::sin(rad) * original_z;
    point.z = -std::sin(rad) * original_x + std::cos(rad) * original_z; // Note the minus sign correction for standard rotation

    // Rotate around Z-axis
    rad = z;
    original_x = point.x; // Use the point.x updated from Y rotation
    original_y = point.y; // Use the point.y updated from X rotation
    point.x = std::cos(rad) * original_x - std::sin(rad) * original_y;
    point.y = std::sin(rad) * original_x + std::cos(rad) * original_y;
}


void line(Screen& screen, float x1, float y1, float x2, float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;

    float length = std::sqrt((dx * dx) + (dy * dy));
    float angle = std::atan2(dy, dx);

    for(float i = 0; i < length; i++){
        screen.pixel(x1 + std::cos(angle) * i,
                     y1 + std::sin(angle) * i);
    }
}

int main(){
    
    Screen screen;

    std::vector<vec3> points{
        {100, 100, 100},
        {200, 100, 100},
        {200, 200, 100},
        {100, 200, 100},

        {100, 100, 200},
        {200, 100, 200},
        {200, 200, 200},
        {100, 200, 200}
    };

    std::vector<connection> connections{
        {0, 4},
        {1, 5}, 
        {2, 6}, 
        {3, 7},

        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},

        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4}
    };

    // Calculate centroid
    vec3 centroid{0, 0, 0};
    for (auto&  p : points){
        centroid.x += p.x;
        centroid.y += p.y;
        centroid.z += p.z;
    }

    centroid.x /= points.size();
    centroid.y /= points.size();
    centroid.z /= points.size();

    while(true){

        for(auto& p : points){
            p.x -= centroid.x;
            p.y -= centroid.y;
            p.z -= centroid.z;
            rotate(p, 0.02, 0.01, 0.04);
            p.x += centroid.x;
            p.y += centroid.y;
            p.z += centroid.z;
            screen.pixel(p.x, p.y);
        }
        for (auto& conn : connections){
            line(screen , points[conn.a].x,
                          points[conn.a].y,
                          points[conn.b].x,
                          points[conn.b].y);
        }
        screen.show();
        screen.clear();
        screen.input();
        SDL_Delay(30);
    }
    return 0;
}