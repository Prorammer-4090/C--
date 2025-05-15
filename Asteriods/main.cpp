#include <iostream>
#include <string>
#include <algorithm>
#include "screen.h"
#include <SDL2/SDL.h>
#include <cmath>
#include "input.h"
#include "ui.h"

using namespace std;
const int SCREENWIDTH = 800;
const int SCREENHEIGHT = 800;

const string TURN_LEFT = "A";
const string TURN_RIGHT = "D";
const string MOVE = "W";
const string FIRE = "Space";

vector<pair<float, float>> vecModelAsteroid;
vector<pair<float, float>> shipCoords;

struct sSpaceObject {
    float x;
    float y;
    float dx;
    float dy;
    int   nSize;
    float angle;
    bool  alive = true;    // <— new flag
};

vector<sSpaceObject> vecAsteroids;
sSpaceObject player;
vector<sSpaceObject> vecBullets;

int score = 0;

class Asteriods : public Screen {
    public:
        Asteriods() : Screen(SCREENWIDTH, SCREENHEIGHT) {
            cout << "Asteriods game initialized." << endl;
            // spawn asteroids at start
            int verts = 20;
            for (int i = 0; i < verts; i++){
                float radius = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
                float a = (float)i / (float)verts * (2 * M_PI);
                vecModelAsteroid.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
            }

            // Create some actual asteroid objects
            vecAsteroids.push_back({20.0f, 20.0f, 3.0f, -3.0f, 64, 0.0f});
            vecAsteroids.push_back({SCREENWIDTH - 50.0f, 100.0f, -3.0f, 3.0f, 64, 0.0f});
            vecAsteroids.push_back({100.0f, SCREENHEIGHT - 50.0f, 3.0f, 3.0f, 64, 0.0f});

            player.x = SCREENWIDTH / 2.0f;
            player.y = SCREENHEIGHT / 2.0f;
            player.dx = 0.0f;
            player.dy = 0.0f;
            player.angle = 0.0f;

            Input input;
            
            // Initialize UI manager
            uiManager = new UIManager(renderer, SCREENWIDTH, SCREENHEIGHT);
            
            // Add score label
            scoreLabel = uiManager->addElement<Label>(20, 20, "Score: 0", 
                                                    SDL_Color{255, 255, 255, 255}, 24);
            
            // Add restart button (initially invisible)
            restartButton = uiManager->addElement<Button>(
                SCREENWIDTH/2 - 100, SCREENHEIGHT/2 - 25, 200, 50, 
                "Restart Game", 
                [this]() { 
                    this->resetGame();
                },
                SDL_Color{50, 200, 50, 255},
                SDL_Color{100, 255, 100, 255}
            );
            restartButton->visible = false;
        }
        
        ~Asteriods() {
            delete uiManager;
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
            player.dx = 0.0f;
            player.dy = 0.0f;
            player.angle = 0.0f;
            player.alive = true;
            
            // Clear asteroids and create new ones
            vecAsteroids.clear();
            vecAsteroids.push_back({20.0f, 20.0f, 3.0f, -3.0f, 64, 0.0f});
            vecAsteroids.push_back({SCREENWIDTH - 50.0f, 100.0f, -3.0f, 3.0f, 64, 0.0f});
            vecAsteroids.push_back({100.0f, SCREENHEIGHT - 50.0f, 3.0f, 3.0f, 64, 0.0f});
            
            // Clear bullets
            vecBullets.clear();
            
            // Reset score
            score = 0;
            scoreLabel->setText("Score: 0");
            
            // Hide restart button
            restartButton->visible = false;
        }
    
    protected:
        // just update positions
        void update() override {
            // Update UI
            uiManager->update(input);

            if (player.alive == false){
                // Show restart button when player is dead
                restartButton->visible = true;
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
        }

        // do all your actual rendering here
        void show() override {
            // clear screen once per frame
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // draw asteroids
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

            for (auto &a : vecAsteroids) {
                // Use the nSize as the scale factor instead of 1.0f
                DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, a.nSize, a.nSize, a);
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
                                a.nSize/2, 0.0f, true
                            });
                            newAsteroids.push_back({a.x, a.y,
                                2.5f * sinf(angle2),
                                2.5f * cosf(angle2),
                                a.nSize/2, 0.0f, true
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

                vecAsteroids.push_back({30.0f * sinf(player.angle - M_PI / 2.0f), 
                                        30.0f * cosf(player.angle - M_PI / 2.0f), 
                                        2.5f * sinf(player.angle),
                                        2.5f * cosf(player.angle),
                                        64, 0.0f});

                vecAsteroids.push_back({30.0f * sinf(player.angle + M_PI / 2.0f), 
                                        30.0f * cosf(player.angle + M_PI / 2.0f), 
                                        2.5f * sinf(-player.angle),
                                        2.5f * cosf(-player.angle),
                                        64, 0.0f});

                vecAsteroids.push_back({30.0f * sinf(player.angle + M_PI / 3.0f), 
                                        30.0f * cosf(player.angle - M_PI / 3.0f), 
                                        2.5f * sinf(-player.angle),
                                        2.5f * cosf(-player.angle),
                                        64, 0.0f});  
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
            
            SDL_RenderPresent(renderer);

            // simple frame cap (~60fps)
            SDL_Delay(16);
        }

        void WrapCoords(float xi, float yi, float &ox, float &oy) {
            // wrap X into [0, SCREENWIDTH)
            ox = fmod(xi, (float)SCREENWIDTH);
            if (ox < 0.0f) ox += SCREENWIDTH;
            // wrap Y into [0, SCREENHEIGHT)
            oy = fmod(yi, (float)SCREENHEIGHT);
            if (oy < 0.0f) oy += SCREENHEIGHT;
        }

        void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoords, float x, float y, float r, float s, float radius, sSpaceObject object, short col = 255){
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
};

int main(){
    Asteriods asteroids;
    asteroids.run();
    return 0;
}