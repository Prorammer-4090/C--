#include <SDL2/SDL.h>
#include <iostream> // For error reporting
#include <vector>
#include <algorithm>

int main() { // Use the standard main signature

    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;
    const int TEXTURE_WIDTH = 1280;
    const int TEXTURE_HEIGHT = 960;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a standard 640x480 window
    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
         std::cerr << "Window and Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
         SDL_Quit();
         return 1;
    }

    SDL_Rect source{0, 0, SCREEN_WIDTH/32*4, SCREEN_HEIGHT/32*4};
    SDL_Rect dest{10, 10, SCREEN_WIDTH-30, SCREEN_HEIGHT-30};

    bool quit = false;
    SDL_Event e;

    auto texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        SCREEN_WIDTH, SCREEN_HEIGHT
    );

    std::vector<SDL_Point> pv;
    for(int i = 0; i < 10000; ++i){
        pv.push_back({rand() % TEXTURE_WIDTH, rand() % TEXTURE_HEIGHT});
    }
    // --- Game Loop ---
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                switch(e.key.keysym.sym){
                    case SDLK_UP: source.y -= 3; break;
                    case SDLK_DOWN: source.y += 3; break;
                    case SDLK_LEFT: source.x -= 3; break;
                    case SDLK_RIGHT: source.x += 3; break;
                    case SDLK_1: source.w *= 2; source.h *= 2; break;
                    case SDLK_2: source.w /= 2; source.h /= 2; break;
                }
            }
        }

        // --- Drawing ---
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Move position of dots around
        std::for_each(pv.begin(),pv.end(),[](auto& item){
            item.x += rand() % 3 - 1;
            item.y += rand() % 3 - 1;
        });

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawPoints(renderer, pv.data(), pv.size());

        // Copy selection from texture to screen
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, &source, &dest);

        // Update screen
        SDL_RenderPresent(renderer);
        // Small delay to prevent high CPU usage (optional)
        SDL_Delay(50);
    }
    // --- End Game Loop ---


    // Destroy window/renderer and quit SDL subsystems
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;

    SDL_Quit();

    return 0;
}