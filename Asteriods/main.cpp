#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector> 
#include <sstream> 
#include "screen.h"
#include <SDL2/SDL.h>
#include <cmath>
#include "input.h"
#include "ui.h"
#include "menus.h"
#include "bezier_shapes.h"

using namespace std;
const int SCREENWIDTH = 800;
const int SCREENHEIGHT = 800;

const string TURN_LEFT = "A";
const string TURN_RIGHT = "D";
const string MOVE = "W";
const string FIRE = "Space";

vector<pair<float, float>> shipCoords;

struct sSpaceObject {
    float x;
    float y;
    float dx;
    float dy;
    int   nSize;
    float angle;
    bool  alive = true;
    std::vector<std::pair<float, float>> model; // Added for individual asteroid shapes
};

vector<sSpaceObject> vecAsteroids;
sSpaceObject player;
vector<sSpaceObject> vecBullets;
fstream file;

int score = 0;

class Asteriods : public Screen {
    public:
        enum GameState {
            MAIN_MENU,
            GAME_RUNNING,
            GAME_PAUSED,
            CONTROLS_MENU,
            VOLUME_MENU,
            HIGHSCORE_MENU,
            GAME_OVER,            // New state
            HIGHSCORE_RECORDING   // New state
        };
        
        Asteriods() : Screen(SCREENWIDTH, SCREENHEIGHT) {
            cout << "Asteriods game initialized." << endl;
            
            // Initialize game state
            currentState = MAIN_MENU;
            
            // Initialize menus
            menus = new Menus(renderer, SCREENWIDTH, SCREENHEIGHT);
            setupMenus();

            // instead of hard‐coding positions, pick 3 random safe ones:
            const float safeRadius = 100.0f;  // no‐spawn circle around player
            for (int i = 0; i < 3; ++i) {
                auto [sx, sy] = getSafeSpawn(safeRadius);
                float ang = float(rand())/RAND_MAX * 2.0f * M_PI;
                vecAsteroids.push_back({ sx, sy,
                                         3.0f * sinf(ang),
                                         3.0f * cosf(ang),
                                         64, 0.0f, true,
                                         generateAsteroidModel() }); // Generate model
            }

            player.x = SCREENWIDTH / 2.0f;
            player.y = SCREENHEIGHT / 2.0f;
            player.dx = 1.0f;
            player.dy = 1.0f;
            player.angle = float((rand())/RAND_MAX) * 2.0f * M_PI;

            Input input;
            
            // Initialize UI manager
            uiManager = new UIManager(renderer, SCREENWIDTH, SCREENHEIGHT);
            
            // Add score label
            scoreLabel = uiManager->addElement<Label>(20, 20, "Score: 0", 
                                                    SDL_Color{255, 255, 255, 255}, 24, "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");
            
            // Add restart button (initially invisible)
            restartButton = uiManager->addElement<Button>(
                SCREENWIDTH/2 - 100, SCREENHEIGHT/2 - 25, 250, 50, 
                "Restart Game", 
                [this]() { 
                    this->resetGame();
                    currentState = GAME_RUNNING; 
                },
                SDL_Color{50, 200, 50, 255},
                SDL_Color{100, 255, 100, 255},
                "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
            );
            restartButton->visible = false;
        }
        
        ~Asteriods() {
            delete uiManager;
            delete menus;
        }

        void run() {
            Screen::run();
        }
        void start(){
            cout << "Asteriods game started." << endl;
        }
        void stop(){
            cout << "Asteriods game stopped." << endl;
            player.x = SCREENWIDTH / 2.0f;
            player.y = SCREENHEIGHT / 2.0f;
            player.dx = 0.0f;
            player.dy = 0.0f;
            player.angle = 0.0f;
            player.alive = false;

            for (auto &a : vecAsteroids){
                a.dx = 0.0f;
                a.dy = 0.0f;
            }

            for (auto &b : vecBullets){
                b.dx = 0.0f;
                b.dy = 0.0f;
            }
        }
        void play(){
            cout << "Playing Asteriods game." << endl;
        }
        void pause(){
            cout << "Asteriods game paused." << endl;
        }

        void reset(){

        }
        
        void resetGame() {
            // Reset player
            player.x = SCREENWIDTH / 2.0f;
            player.y = SCREENHEIGHT / 2.0f;
            player.dx = 1.0f;
            player.dy = 1.0f;
            player.angle = float(rand())/RAND_MAX * 2.0f * M_PI;
            player.alive = true;
            
            // Clear asteroids and create new ones
            vecAsteroids.clear();
            const float safeRadius = 100.0f;  // no‐spawn circle around player
            for (int i = 0; i < 3; ++i) {
                auto [sx, sy] = getSafeSpawn(safeRadius);
                float ang = float(rand())/RAND_MAX * 2.0f * M_PI;
                vecAsteroids.push_back({ sx, sy,
                                         3.0f * sinf(ang),
                                         3.0f * cosf(ang),
                                         64, 0.0f, true,
                                         generateAsteroidModel() }); // Generate model
            }
            
            // Clear bullets
            vecBullets.clear();
            
            // Reset score
            score = 0;
            scoreLabel->setText("Score: 0");
            
            // Hide restart button
            restartButton->visible = false;
        }
    
    protected:
        void update() override {
            // Update input handling for all UI elements
            uiManager->update(input);
            
            // Update the menu system (this was missing)
            menus->update(input);
            
            // Handle different game states
            switch (currentState) {
                case MAIN_MENU:
                    // Only handle menu interactions in main menu state
                    // Game physics are paused
                    menuBackgroundUpdate();
                    break;
                    
                case GAME_RUNNING:
                    // Only update game physics when in running state
                    updateGameplay();
                    break;
                    
                case GAME_PAUSED:
                    // Game is paused, only handle menu interactions
                    break;
                    
                case CONTROLS_MENU:
                    menuBackgroundUpdate();
                    break;
                case VOLUME_MENU:
                    menuBackgroundUpdate();
                    break;
                case HIGHSCORE_MENU:
                    // Just handle menu interactions
                    menuBackgroundUpdate();
                    break;
                case GAME_OVER:
                    // Handle Game Over menu interactions
                    menuBackgroundUpdate();
                    break;
                case HIGHSCORE_RECORDING:
                    // Handle Highscore Recording menu interactions and text input
                    menuBackgroundUpdate();
                    handleNameInput();
                    break;
            }
        }

        void menuBackgroundUpdate() {
            // Update existing asteroids
            for (auto &a : vecAsteroids) {
                a.x += a.dx * (deltaTime + 0.125);
                a.y -= a.dy * (deltaTime + 0.125);
                a.angle += 0.0625 * (deltaTime + 0.125);
                WrapCoords(a.x, a.y, a.x, a.y);
            }
            
            // Spawn new asteroids occasionally (0.1% chance per frame)
            if (rand() % 100 < 0.1 && vecAsteroids.size() < 20) {  // Limit to 20 asteroids max
                float ang = float(rand()) / RAND_MAX * 2.0f * M_PI;
                
                // Spawn from edges
                float x, y;
                int side = rand() % 4;
                switch(side) {
                    case 0: // top
                        x = rand() % SCREENWIDTH;
                        y = -20;
                        break;
                    case 1: // right
                        x = SCREENWIDTH + 20;
                        y = rand() % SCREENHEIGHT;
                        break;
                    case 2: // bottom
                        x = rand() % SCREENWIDTH;
                        y = SCREENHEIGHT + 20;
                        break;
                    case 3: // left
                        x = -20;
                        y = rand() % SCREENHEIGHT;
                        break;
                }
                
                vecAsteroids.push_back({ 
                    x, y,
                    2.0f * sinf(ang),
                    2.0f * cosf(ang),
                    16 + rand() % 48, // Random size between 16 and 64
                    0.0f, true,
                    generateAsteroidModel() // Generate model
                });
            }
            
            // Remove asteroids when there are too many
            if (vecAsteroids.size() > 20) {
                vecAsteroids.erase(vecAsteroids.begin());
            }
        }

        void show() override {
            // clear screen once per frame
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            // Render asteroids in the background for menu screens
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            for (auto &a : vecAsteroids) {
                DrawWireFrameModel(a.model, a.x, a.y, a.angle, a.nSize, a.nSize, a); // Use a.model
            }
            
            switch (currentState) {
                case MAIN_MENU:
                    // Display main menu (with all other menus hidden)
                    menus->showMainMenu();
                    break;
                    
                case GAME_RUNNING:
                    // Render the game
                    renderGameplay();
                    break;
                    
                case GAME_PAUSED:
                    // Render the game (dimmed) with pause menu on top
                    renderGameplay();
                    menus->showPauseMenu();
                    break;
                    
                case CONTROLS_MENU:
                    menus->showControlsMenu(previousState == GAME_PAUSED);
                    break;
                    
                case VOLUME_MENU:
                    menus->showVolumeMenu(previousState == GAME_PAUSED);
                    break;
                    
                case HIGHSCORE_MENU:
                    menus->showHighscoreMenu(getHighScores());
                    break;
                    
                case GAME_OVER:
                    menus->showGameOverMenu();
                    break;
                    
                case HIGHSCORE_RECORDING:
                    menus->showHighscoreRecordingMenu();
                    break;
            }
            
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        
        void updateGameplay() {
            if (player.alive == false){
                // Transition to Game Over state
                if (currentState != GAME_OVER && currentState != HIGHSCORE_RECORDING) {
                    showGameOverScreen();
                }
                restartButton->visible = false;
                return;
            }

            for (auto &a : vecAsteroids) {
                a.x += a.dx * (deltaTime+0.125);
                a.y -= a.dy * (deltaTime+0.125);
                a.angle += 0.0625 * (deltaTime+0.125);
                WrapCoords(a.x, a.y, a.x, a.y);
            }
            // cout << input.keyHeld(TURN_LEFT) << endl;
            if (input.keyHeld(TURN_LEFT)){
                player.angle -= 1.0f * (deltaTime+0.125);
            }
            if (input.keyHeld(TURN_RIGHT)){
                player.angle -= -1.0f * (deltaTime+0.125);
            }

            if (input.keyHeld(MOVE)){
                // Thrust in heading direction:
                float accel = 2.5f * (deltaTime + 0.125f);
                player.dx +=  sinf(player.angle) * accel;
                player.dy += -cosf(player.angle) * accel;
            }

            player.x += player.dx * (deltaTime+0.125);
            player.y += player.dy * (deltaTime+0.125);

            WrapCoords(player.x, player.y, player.x, player.y);

            for (auto &a : vecAsteroids){
                if (Collide(a.x, a.y, a.nSize, player.x, player.y)){
                    player.alive = false;
                    return;
                }
            }

            // Fire Bullets - only when key is first pressed, not held
            if (input.keyDown(FIRE)) {
                // Create a bullet with a size of 5 (was 0)
                cout << "bullet fired" << endl;
                vecBullets.push_back({
                    player.x, 
                    player.y, 
                    50.0f * sinf(player.angle), 
                    50.0f * cosf(player.angle), 
                    5, // Size was 0, which makes them invisible
                    player.angle
                });
            }
            
            for (auto &b : vecBullets) {
                b.x += b.dx * (deltaTime+0.125);
                b.y -= b.dy * (deltaTime+0.125);
            }

            // Update score label
            scoreLabel->setText("Score: " + to_string(score));
            
            // Handle pausing the game
            if (input.keyDown("Escape") || input.keyDown("P")) {
                previousState = GAME_RUNNING;
                currentState = GAME_PAUSED;
            }
        }
        
        void renderGameplay() {
            // draw asteroids
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

            for (auto &a : vecAsteroids) {
                // Use the nSize as the scale factor instead of 1.0f
                DrawWireFrameModel(a.model, a.x, a.y, a.angle, a.nSize, a.nSize, a); // Use a.model
            }

            vector<sSpaceObject> newAsteroids;
            // now draw remaining bullets & handle collisions
            for (auto &b : vecBullets) {
                for (auto &a : vecAsteroids) {
                    if (Collide(a.x, a.y, a.nSize, b.x, b.y)) {
                        // mark both objects dead
                        a.alive = false;
                        b.alive = false;

                        // Split Asteroids
                        if (a.nSize > 16) {
                            float angle1 = ((float)rand() / (float)RAND_MAX) * (2 * M_PI);
                            float angle2 = ((float)rand() / (float)RAND_MAX) * (2 * M_PI);
                            newAsteroids.push_back({a.x, a.y,
                                2.5f * sinf(angle1),
                                2.5f * cosf(angle1),
                                a.nSize/2, 0.0f, true,
                                generateAsteroidModel() // Generate model for split piece
                            });
                            newAsteroids.push_back({a.x, a.y,
                                2.5f * sinf(angle2),
                                2.5f * cosf(angle2),
                                a.nSize/2, 0.0f, true,
                                generateAsteroidModel() // Generate model for split piece
                            });
                            score += 100;
                        }
                    }
                }
                SDL_Rect bulletRect {
                    int(b.x - b.nSize/2),
                    int(b.y - b.nSize/2),
                    b.nSize,
                    b.nSize
                };
                SDL_RenderFillRect(renderer, &bulletRect);
            }

            // merge in new half‐rocks
            for (auto &na : newAsteroids)
                vecAsteroids.push_back(na);

            // finally, remove any dead bullets / asteroids
            vecBullets.erase(
                std::remove_if(vecBullets.begin(), vecBullets.end(),
                    [](const sSpaceObject &b){ return !b.alive || b.x < 0 || b.x > SCREENWIDTH
                        || b.y < 0 || b.y > SCREENHEIGHT; }
                ),
                vecBullets.end()
            );
            vecAsteroids.erase(
                std::remove_if(vecAsteroids.begin(), vecAsteroids.end(),
                    [](const sSpaceObject &a){ return !a.alive; }
                ),
                vecAsteroids.end()
            );

            if (vecAsteroids.empty()){
                score += 1000;
                const float safeRadius = 150.0f;  // maybe larger on later waves
                for (int i = 0; i < 3; ++i) {
                    auto [sx, sy] = getSafeSpawn(safeRadius);
                    float ang = float(rand())/RAND_MAX * 2.0f * M_PI;
                    vecAsteroids.push_back({ sx, sy,
                                             2.5f * sinf(ang),
                                             2.5f * cosf(ang),
                                             64, 0.0f, true,
                                             generateAsteroidModel() }); // Generate model
                }
            }


            if (player.alive) {// Draw Ship
            // Ship vertices - defined relative to ship's center (0,0)
            shipCoords = {{0.0f, -20.0f}, {-10.0f, 20.0f}, {10.0f, 20.0f}};
            float shipRadius = 20.0f; // Approximate ship size
            // Call DrawWireFrameModel to draw the ship
            DrawWireFrameModel(shipCoords, player.x, player.y, player.angle, 0.5f, shipRadius, player);
        }
            
            // Update UI rendering at the end (to appear on top)
            uiManager->render();
        }

        void WrapCoords(float xi, float yi, float &ox, float &oy) {
            // wrap X into [0, SCREENWIDTH)
            ox = fmod(xi, (float)SCREENWIDTH);
            if (ox < 0.0f) ox += SCREENWIDTH;
            // wrap Y into [0, SCREENHEIGHT)
            oy = fmod(yi, (float)SCREENHEIGHT);
            if (oy < 0.0f) oy += SCREENHEIGHT;
        }

        void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoords, float x, float y, float r, float s, float radius, sSpaceObject object){
            vector<pair<float, float>> vecTransformCoords;
            int verts = vecModelCoords.size();
            vecTransformCoords.resize(verts);

            // Rotate
            for (int i = 0; i < verts; i++){
                vecTransformCoords[i].first = vecModelCoords[i].first * cosf(r) - vecModelCoords[i].second * sinf(r);
                vecTransformCoords[i].second = vecModelCoords[i].first * sinf(r) + vecModelCoords[i].second * cosf(r);
            }

            // Scale
            for (int i = 0; i < verts; i++){
                vecTransformCoords[i].first = vecTransformCoords[i].first * s;
                vecTransformCoords[i].second = vecTransformCoords[i].second * s;
            }

            // Translate
            for (int i = 0; i < verts; i++){
                vecTransformCoords[i].first = vecTransformCoords[i].first + x;
                vecTransformCoords[i].second = vecTransformCoords[i].second + y;
            }

            // Draw Shape
            for (int i = 0; i < verts; i++){
                int j = (i + 1) % verts;
                SDL_RenderDrawLine(
                    renderer,
                    int(vecTransformCoords[i].first), int(vecTransformCoords[i].second),
                    int(vecTransformCoords[j].first), int(vecTransformCoords[j].second)
                );
            }
            
            // Handle wrapping
            WrapModel(radius, vecTransformCoords, object);
        }

        private: void WrapModel(float radius, vector<pair<float, float>> transformCoords, sSpaceObject object){
            bool nearRightEdge = false, nearLeftEdge = false;
            bool nearTopEdge = false, nearBottomEdge = false;
            if (object.x < radius) nearLeftEdge = true;
            if (object.x > SCREENWIDTH - radius) nearRightEdge = true;
            if (object.y < radius) nearTopEdge = true;
            if (object.y > SCREENHEIGHT - radius) nearBottomEdge = true;

            int verts = transformCoords.size();
            
            // Draw wrapped model when near edges
            if (nearRightEdge) {
                // Draw wrapped model on left side
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first - SCREENWIDTH), 
                        int(transformCoords[i].second),
                        int(transformCoords[j].first - SCREENWIDTH), 
                        int(transformCoords[j].second)
                    );
                }
            }
            
            if (nearLeftEdge) {
                // Draw wrapped model on right side
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first + SCREENWIDTH), 
                        int(transformCoords[i].second),
                        int(transformCoords[j].first + SCREENWIDTH), 
                        int(transformCoords[j].second)
                    );
                }
            }
            
            if (nearBottomEdge) {
                // Draw wrapped model on top side
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first), 
                        int(transformCoords[i].second - SCREENHEIGHT),
                        int(transformCoords[j].first), 
                        int(transformCoords[j].second - SCREENHEIGHT)
                    );
                }
            }
            
            if (nearTopEdge) {
                // Draw wrapped model on bottom side
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first), 
                        int(transformCoords[i].second + SCREENHEIGHT),
                        int(transformCoords[j].first), 
                        int(transformCoords[j].second + SCREENHEIGHT)
                    );
                }
            }
            
            // Corner cases - when object is near two edges at once
            if (nearRightEdge && nearBottomEdge) {
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first - SCREENWIDTH), 
                        int(transformCoords[i].second - SCREENHEIGHT),
                        int(transformCoords[j].first - SCREENWIDTH), 
                        int(transformCoords[j].second - SCREENHEIGHT)
                    );
                }
            }
            
            if (nearLeftEdge && nearBottomEdge) {
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first + SCREENWIDTH), 
                        int(transformCoords[i].second - SCREENHEIGHT),
                        int(transformCoords[j].first + SCREENWIDTH), 
                        int(transformCoords[j].second - SCREENHEIGHT)
                    );
                }
            }
            
            if (nearRightEdge && nearTopEdge) {
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first - SCREENWIDTH), 
                        int(transformCoords[i].second + SCREENHEIGHT),
                        int(transformCoords[j].first - SCREENWIDTH), 
                        int(transformCoords[j].second + SCREENHEIGHT)
                    );
                }
            }
            
            if (nearLeftEdge && nearTopEdge) {
                for (int i = 0; i < verts; ++i) {
                    int j = (i + 1) % verts;
                    SDL_RenderDrawLine(
                        renderer,
                        int(transformCoords[i].first + SCREENWIDTH), 
                        int(transformCoords[i].second + SCREENHEIGHT),
                        int(transformCoords[j].first + SCREENWIDTH), 
                        int(transformCoords[j].second + SCREENHEIGHT)
                    );
                }
            }
        }

        bool Collide(float cx, float cy, float radius, float x, float y){
            return sqrt((x - cx)*(x - cx) + (y - cy)*(y - cy)) < radius;
        }

    private:
        UIManager* uiManager;
        Label* scoreLabel;
        Button* restartButton;
        Menus* menus;
        GameState currentState;
        GameState previousState;

        std::vector<std::pair<float, float>> generateAsteroidModel() {
            std::vector<std::pair<float, float>> new_model;
            // Vary parameters for diverse shapes
            int num_base_points = 5 + rand() % 5; // 5 to 9 base points
            double model_rad_factor = 0.2 + static_cast<double>(rand()) / RAND_MAX * 0.5; // 0.2 to 0.7
            double model_edgy_factor = 0.1 + static_cast<double>(rand()) / RAND_MAX * 0.6; // 0.1 to 0.7
            // The number of points in the final Bezier curve is determined by get_curve's defaults

            std::vector<Point> random_shape_pts = get_random_points(num_base_points, 1.0, 0.1); // Added mindst_param
            BezierCurveResult bezier_result = get_bezier_curve(random_shape_pts, model_rad_factor, model_edgy_factor);

            if (!bezier_result.x_coords.empty() && bezier_result.x_coords.size() >= 3) {
                std::vector<Point> model_points_double;
                Point centroid = {0.0, 0.0};
                for (size_t i = 0; i < bezier_result.x_coords.size(); ++i) {
                    Point p = {bezier_result.x_coords[i], bezier_result.y_coords[i]};
                    model_points_double.push_back(p);
                    centroid.x += p.x;
                    centroid.y += p.y;
                }
                if (!model_points_double.empty()) {
                    centroid.x /= model_points_double.size();
                    centroid.y /= model_points_double.size();
                }


                double max_mag = 0.0;
                for (size_t i = 0; i < model_points_double.size(); ++i) {
                    model_points_double[i].x -= centroid.x;
                    model_points_double[i].y -= centroid.y;
                    double mag = model_points_double[i].mag();
                    if (mag > max_mag) {
                        max_mag = mag;
                    }
                }

                if (max_mag > 1e-6) { // Avoid division by zero
                    for (const auto& p_double : model_points_double) {
                        new_model.push_back(std::make_pair(
                            static_cast<float>(p_double.x / max_mag),
                            static_cast<float>(p_double.y / max_mag)
                        ));
                    }
                }
            }

            // Fallback if Bezier generation fails or results in too few points
            if (new_model.empty() || new_model.size() < 3) {
                new_model.clear();
                int verts = 8 + rand() % 8; // Fallback to a random-ish polygon (8-15 verts)
                for (int i = 0; i < verts; i++){
                    // Generate points with somewhat random radius for a less circular fallback
                    float radius_factor = 0.7f + static_cast<float>(rand()) / RAND_MAX * 0.6f; // 0.7 to 1.3
                    float a = (float)i / (float)verts * (2 * M_PI);
                    new_model.push_back(std::make_pair(radius_factor * sinf(a), radius_factor * cosf(a)));
                }
                // Normalize this fallback polygon too
                Point centroid_fallback = {0.0, 0.0};
                if (!new_model.empty()){
                    for(const auto& p_pair : new_model) {
                        centroid_fallback.x += p_pair.first;
                        centroid_fallback.y += p_pair.second;
                    }
                    centroid_fallback.x /= new_model.size();
                    centroid_fallback.y /= new_model.size();
                }

                std::vector<Point> temp_points;
                double max_mag_fallback = 0.0;
                for(const auto& p_float_pair : new_model) {
                    Point p_temp = {p_float_pair.first - centroid_fallback.x, p_float_pair.second - centroid_fallback.y};
                    temp_points.push_back(p_temp);
                    double mag = p_temp.mag();
                    if (mag > max_mag_fallback) max_mag_fallback = mag;
                }

                if (max_mag_fallback > 1e-6) {
                    new_model.clear(); // Clear before repopulating with normalized points
                    for(const auto& p_double : temp_points) {
                         new_model.push_back(std::make_pair(
                            static_cast<float>(p_double.x / max_mag_fallback),
                            static_cast<float>(p_double.y / max_mag_fallback)
                        ));
                    }
                } else if (new_model.empty()) { // Absolute fallback: a simple square
                     new_model.push_back({-1.0f, -1.0f});
                     new_model.push_back({ 1.0f, -1.0f});
                     new_model.push_back({ 1.0f,  1.0f});
                     new_model.push_back({-1.0f,  1.0f});
                }
            }
            return new_model;
        }

        void setupMenus() {
            // Set up main menu callbacks
            vector<function<void()>> mainMenuCallbacks = {
                [this]() { startGame(); },             // Start
                [this]() { showControls(false); },     // Controls
                [this]() { showVolume(false); },       // Volume
                [this]() { showHighScores(); },        // Highscore
                [this]() { quitGame(); }               // Quit
            };
            
            // Set up pause menu callbacks
            vector<function<void()>> pauseMenuCallbacks = {
                [this]() { resumeGame(); },            // Resume
                [this]() { resetGame(); currentState = GAME_RUNNING; },  // Restart
                [this]() { showControls(true); },      // Controls
                [this]() { showVolume(true); },        // Volume
                [this]() { returnToMainMenu(); }       // Main Menu
            };
            
            // Set up volume callbacks
            vector<function<void()>> volumeCallbacks = {
                [this]() { increaseVolume(); },        // Increase volume
                [this]() { decreaseVolume(); },        // Decrease volume
            };

            // Set up Game Over menu callbacks
            vector<function<void()>> gameOverMenuCallbacks = {
                [this]() { showHighscoreRecordingScreen(); }, // "Submit Score" button
                [this]() { returnToMainMenu(); }              // "Main Menu" button
            };

            // Callback for submitting name from highscore recording menu
            auto highscoreSubmitAction = [this](string playerName) {
                saveHighScore(playerName, score);
                returnToMainMenu(); // After submitting, go to main menu
            };
            
            // Store callbacks for later recreation
            menus->setMainMenuCallbacks(mainMenuCallbacks);
            menus->setPauseMenuCallbacks(pauseMenuCallbacks);
            menus->setVolumeCallbacks(volumeCallbacks);
            menus->setGameOverMenuCallbacks(gameOverMenuCallbacks); // Store game over callbacks
            menus->setHighscoreRecordingMenuCallbacks(highscoreSubmitAction); // Store highscore submit callback
            
            // Set up back button handlers
            menus->setBackHandlers(
                // Controls menu back handler
                [this](bool fromPauseMenu) {
                    if (fromPauseMenu) {
                        currentState = GAME_PAUSED;
                        cout << "Going back to pause menu from controls" << endl;
                    } else {
                        currentState = MAIN_MENU;
                        cout << "Going back to main menu from controls" << endl;
                    }
                },
                // Volume menu back handler
                [this](bool fromPauseMenu) {
                    if (fromPauseMenu) {
                        currentState = GAME_PAUSED;
                        cout << "Going back to pause menu from volume" << endl;
                    } else {
                        currentState = MAIN_MENU;
                        cout << "Going back to main menu from volume" << endl;
                    }
                },
                // Highscore menu back handler
                [this]() {
                    currentState = MAIN_MENU;
                }
            );
            
            if (file.is_open()) {
                
            }
            
            // Initialize all menus
            menus->initMainMenu(mainMenuCallbacks);
            menus->initPauseMenu(pauseMenuCallbacks);
            menus->initVolumeMenu(volumeCallbacks);
            menus->initGameOverMenu(gameOverMenuCallbacks); // Initialize game over menu
            menus->initHighscoreRecordingMenu(highscoreSubmitAction); // Initialize highscore recording menu
        }
        
        void startGame() {
            resetGame();
            currentState = GAME_RUNNING;
        }
        
        void resumeGame() {
            currentState = GAME_RUNNING;
        }

        void showGameOverScreen() {
            currentState = GAME_OVER;
        }

        void showHighscoreRecordingScreen() {
            previousState = GAME_OVER;
            currentState = HIGHSCORE_RECORDING;
            menus->clearCurrentNameInput();
        }
        
        void showControls(bool fromPause) {
            previousState = fromPause ? GAME_PAUSED : MAIN_MENU;
            currentState = CONTROLS_MENU;
            cout << "Controls menu opened from " << (fromPause ? "pause menu" : "main menu") << endl;
        }
        
        void showVolume(bool fromPause) {
            previousState = fromPause ? GAME_PAUSED : MAIN_MENU;
            currentState = VOLUME_MENU;
            cout << "Volume menu opened from " << (fromPause ? "pause menu" : "main menu") << endl;
        }
        
        void showHighScores() {
            previousState = MAIN_MENU; // Always return to main menu from highscores
            currentState = HIGHSCORE_MENU;
        }
        
        void returnToMainMenu() {
            currentState = MAIN_MENU;
        }
        
        void quitGame() {
            running = false;
        }
        
        vector<string> getHighScores() {
            vector<pair<int, string>> scores; // Store score first for easy sorting
            vector<string> formattedScores;
            ifstream scoreFile("/Users/jethroaiyesan/Programming/Tutorials/C++/Asteriods/highscore.txt");
            string line;

            if (scoreFile.is_open()) {
                while (getline(scoreFile, line)) {
                    stringstream ss(line);
                    string playerName;
                    string scoreStr;
                    string colon; // To consume the " : " part

                    // Assuming format "Player Name" : Score
                    // Read player name (which might contain spaces) enclosed in quotes
                    if (getline(ss, playerName, '"')) { // Consume opening quote
                        if (getline(ss, playerName, '"')) { // Read name until closing quote
                            // Read " : "
                            if (ss >> colon >> scoreStr) { // colon should be ":"
                                try {
                                    int currentScore = stoi(scoreStr);
                                    scores.push_back({currentScore, playerName});
                                } catch (const std::invalid_argument& ia) {
                                    cerr << "Invalid argument: " << ia.what() << " for line: " << line << endl;
                                } catch (const std::out_of_range& oor) {
                                    cerr << "Out of Range error: " << oor.what() << " for line: " << line << endl;
                                }
                            }
                        }
                    }
                }
                scoreFile.close();

                // Sort scores in descending order
                sort(scores.rbegin(), scores.rend());

                // Format for display
                for (const auto& p : scores) {
                    formattedScores.push_back(p.second + " - " + to_string(p.first));
                }

            } else {
                cerr << "Unable to open highscore.txt for reading." << endl;
                formattedScores.push_back("No high scores yet!");
            }

            // Limit to top 10 or less if fewer exist
            if (formattedScores.size() > 10) {
                formattedScores.resize(10);
            }
            
            if (formattedScores.empty()) {
                 formattedScores.push_back("No high scores yet!");
            }

            return formattedScores;
        }
        
        void increaseVolume() {
        }
        
        void decreaseVolume() {
        }

        void saveHighScore(const string& playerName, int playerScore) {
            file.open("/Users/jethroaiyesan/Programming/Tutorials/C++/Asteriods/highscore.txt",
                      std::ios::in | std::ios::out | std::ios::app);
            if (!file.is_open()) {
                std::ofstream create("/Users/jethroaiyesan/Programming/Tutorials/C++/Asteriods/highscore.txt");
                create.close();
                file.open("/Users/jethroaiyesan/Programming/Tutorials/C++/Asteriods/highscore.txt",
                          std::ios::in | std::ios::out | std::ios::app);
            }
            if (file.is_open()) {
                string nameToSave = playerName.empty() ? "Player" : playerName;
                file << "\"" << nameToSave << "\" : " << playerScore << "\n";
                file.close();
                cout << "Highscore saved: " << nameToSave << " - " << playerScore << endl;
            } else {
                cerr << "Error: Could not open highscore.txt for writing." << endl;
            }
        }

        void handleNameInput() {
            if (currentState == HIGHSCORE_RECORDING) {
                for (auto &k : input.getKeysDown()) {
                    if (k == "Return") {
                    } else if (k == "Backspace") {
                        menus->handleTextInput("Backspace");
                    } else if (k == "Space") {
                        menus->handleTextInput("Space");
                    } else if (k.length() == 1) { 
                        char c = k[0];
                        if (isalnum(c)) { 
                             menus->handleTextInput(k);
                        }
                    }
                }
            }
        }

        std::pair<float,float> getSafeSpawn(float safeRadius) {
            float x, y;
            float sqrR = safeRadius*safeRadius;
            do {
                x = float(rand())/RAND_MAX * SCREENWIDTH;
                y = float(rand())/RAND_MAX * SCREENHEIGHT;
            } while ((x-player.x)*(x-player.x) + (y-player.y)*(y-player.y) < sqrR);
            return { x, y };
        }
};

int main(){
    Asteriods asteroids;
    asteroids.run();
    return 0;
}