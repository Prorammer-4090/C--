#include <SDL2/SDL.h>
#include <iostream> // For error reporting
#include <vector>
#include <algorithm>
#include <deque>

int main() { // Use the standard main signature

    const int SCREEN_WIDTH = 1000;
    const int SCREEN_HEIGHT = 1000;

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

    enum Directions{
        NONE, DOWN, LEFT, RIGHT, UP // Add NONE state
    };

    SDL_Rect head {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 10, 10};
    // Snake Body container
    std::deque<SDL_Rect> rq;
    int size = 1;


    // Apple Container
    std::vector<SDL_Rect> apples;

    // Create apples on the map
    for(int i = 0; i < 100; i++){
        apples.push_back({rand()%100*10, rand()%100*10, 10, 10});
    }

    bool quit = false;
    SDL_Event event;
    int dir = NONE; // Initialize dir to NONE

    // --- Game Loop ---
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&event) != 0) {
            // User requests quit
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym)
                {
                    case SDLK_DOWN:
                        dir = DOWN;
                        break;
                    case SDLK_LEFT:
                        dir = LEFT;
                        break;
                    case SDLK_RIGHT:
                        dir = RIGHT;
                        break;
                    case SDLK_UP:
                        dir = UP;
                        break;
                    default:
                        break;
                }

            }
        }

        // Move
        switch (dir)
        {
            case DOWN:
                head.y += 10;
                break;
            case UP:
                head.y -= 10;
                break;
            case LEFT:
                head.x -= 10;
                break;
            case RIGHT:
                head.x += 10; // Corrected: Should move right (increase x)
                break;
            // case NONE and default: do nothing
            default:
                break;
        }

        // Collision Detection with apple
        std::for_each(apples.begin(), apples.end(), [&](auto& apple){
            if (head.x == apple.x && head.y == apple.y){
                size += 10;
                apple.x = -10;
                apple.y = -10;
            }
        });

        // Collision Detection with self
        std::for_each(rq.begin(), rq.end(), [&](auto& snake_segment){
            if (head.x == snake_segment.x && head.y == snake_segment.y){
                size = 1;
                
            }
        });
        // add newest head to snake
        rq.push_front(head);

        while(rq.size() > size){
            rq.pop_back();
        }

        // --- Drawing ---

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

         // Draw body
         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
         std::for_each(rq.begin(), rq.end(), [&](auto& snake_segment){
            SDL_RenderFillRect(renderer, &snake_segment);
         });

        // Draw apples
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
         std::for_each(apples.begin(), apples.end(), [&](auto& apple){
            SDL_RenderFillRect(renderer, &apple);
         });

        // Update screen
        SDL_RenderPresent(renderer);
        // Small delay to prevent high CPU usage (optional)
        SDL_Delay(100); // Increased delay slightly for better visibility
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