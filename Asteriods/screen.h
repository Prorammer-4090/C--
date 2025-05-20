#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "input.h"
#include <iostream>


class Screen {
    SDL_Window* window;
    protected: SDL_Renderer* renderer;
    SDL_Surface* icon = nullptr;
    Input input;
    bool running = true;
    double deltaTime;

    public:
        Screen(int width=800, int height=800){
            if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            }

            if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
                std::cerr << "IMG_Init failed: " << IMG_GetError() << "\n";
            }
        
            if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
                 std::cerr << "Window and Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
                 SDL_Quit();
            }

            icon = IMG_Load("images/asteroids.png");

            if (!icon) {
                std::cerr << "Failed to load icon: " << IMG_GetError() << "\n";
            } else {
                // now set as window icon
                SDL_SetWindowIcon(window, icon);
            }
                      
            SDL_SetWindowTitle(window, "Asteriods");

            input = Input();
        }


        virtual void show(){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_RenderPresent(renderer);
        }


        virtual void update(){
            
        }

        void run(){
            const double FIXED_DELTA = 1.0/60.0;
            double accumulator    = 0.0;
            Uint64 previousTime   = SDL_GetTicks64();

            while(running) {
                input.update();

                Uint64 currentTime = SDL_GetTicks64();
                double frameTime   = (currentTime - previousTime) / 1000.0;
                previousTime       = currentTime;

                if(frameTime > 0.25) frameTime = 0.25;
                accumulator += frameTime;

                while(accumulator >= FIXED_DELTA){
                    // your fixed‐update logic here
                    accumulator -= FIXED_DELTA;
                }
                deltaTime = frameTime;

                update();   // render or variable‐step logic
                show();

                if(input.shouldQuit())
                    running = false;
            }
            // Destroy window/renderer and quit SDL subsystems
            destroy();
        }

        void destroy(){
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            window = nullptr;
            renderer = nullptr;
            SDL_Quit();
        }

};