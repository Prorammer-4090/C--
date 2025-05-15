#include <SDL2/SDL.h>
#include <iostream> // For error reporting

int main() { // Use the standard main signature

    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a standard 640x480 window
    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
         std::cerr << "Window and Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
         SDL_Quit();
         return 1;
    }

    bool quit = false;
    SDL_Event event;

    // --- Game Loop ---
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&event) != 0) {
            // User requests quit
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // --- Drawing ---

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // Update screen
        SDL_RenderPresent(renderer);
        // Small delay to prevent high CPU usage (optional)
        // SDL_Delay(10);
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