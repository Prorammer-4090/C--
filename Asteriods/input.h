#pragma once
#include <SDL2/SDL.h>
#include <string>       // for std::string
#include <vector>
#include <map>
#include <algorithm>    // for std::find

class Input {
    SDL_Event event;
    bool quit = false;
    std::vector<std::string> keysDown, keysHeld, keysUp;
    std::map<std::string,bool> mouseButtons;
  public:
    Input() {
      mouseButtons["left"] = mouseButtons["right"] = mouseButtons["middle"] = false;
    }
    void resetStates() {
      keysDown.clear(); keysUp.clear();
    }
    void update() {
      resetStates();
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = true;
        }
        else if (event.type == SDL_KEYDOWN) {
          auto name = SDL_GetKeyName(event.key.keysym.sym);
          if (std::find(keysHeld.begin(), keysHeld.end(), name) == keysHeld.end()) {
            keysDown.push_back(name);
            keysHeld.push_back(name);
          }
        }
        else if (event.type == SDL_KEYUP) {
          auto name = SDL_GetKeyName(event.key.keysym.sym);
          auto it = std::find(keysHeld.begin(), keysHeld.end(), name);
          if (it != keysHeld.end()) {
            keysHeld.erase(it);
            keysUp.push_back(name);
          }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
          bool down = (event.type == SDL_MOUSEBUTTONDOWN);
          updateMouse(event.button.button, down);
        }
      }
    }
    bool shouldQuit()    const { return quit; }

    bool keyDown(const std::string& k) const { return std::find(keysDown.begin(), keysDown.end(), k) != keysDown.end(); }

    bool keyUp(const std::string& k)   const { return std::find(keysUp.begin(), keysUp.end(), k)   != keysUp.end(); }

    bool keyHeld(const std::string& k) const { return std::find(keysHeld.begin(), keysHeld.end(), k) != keysHeld.end(); }

    std::map<std::string,bool> getMouseStates() const { return mouseButtons; }
    
    const std::vector<std::string>& getKeysDown() const { return keysDown; }
    
  private:
    void updateMouse(int b, bool d) {
      if (b == SDL_BUTTON_LEFT)   mouseButtons["left"]   = d;
      if (b == SDL_BUTTON_MIDDLE) mouseButtons["middle"] = d;
      if (b == SDL_BUTTON_RIGHT)  mouseButtons["right"]  = d;
    }
};