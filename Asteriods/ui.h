#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "input.h"

using namespace std;

// Helper function to normalize SDL_Color
SDL_Color normalizeColor(const SDL_Color& color, int alpha = 255) {
    SDL_Color result = color;
    result.a = alpha;
    return result;
}

// Helper function to convert RGB(A) tuples to SDL_Color
SDL_Color tupleToColor(int r, int g, int b, int a = 255) {
    SDL_Color color = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
    return color;
}

// Base UI Element class
class UIElement {
public:
    UIElement(int x, int y, int width, int height) {
        rect = {x, y, width, height};
        visible = true;
    }
    
    virtual ~UIElement() {}
    
    virtual void update(Input& input) {}
    
    virtual void render(SDL_Renderer* renderer) {}
    
    SDL_Rect rect;
    bool visible;
};

// Button class with 3D effect
class Button : public UIElement {
public:
    Button(int x, int y, int width, int height, const string& text, function<void()> callback,
           SDL_Color color = {255, 255, 255, 255},
           SDL_Color hoverColor = {130, 130, 130, 255},
           string fontPath = "", 
           SDL_Color textColor = {0, 0, 0, 255},
           int elevation = 6, 
           int fontSize = 24)
        : UIElement(x, y, width, height),
          text(text),
          callback(callback),
          color(color),
          hoverColor(hoverColor),
          currentColor(color),
          textColor(textColor),
          elevation(elevation),
          dynamicElevation(elevation),
          originalYPos(y),
          fontSize(fontSize),
          isPressed(false) {
        
        // Create top and bottom rects
        topRect = {x, y - elevation, width, height};
        bottomRect = {x, y, width, height};
        
        // Set bottom color (darker than top)
        bottomColor = {
            static_cast<Uint8>(max(0, color.r - 40)),
            static_cast<Uint8>(max(0, color.g - 40)),
            static_cast<Uint8>(max(0, color.b - 40)),
            color.a
        };
        
        // Load font if provided, otherwise use default
        loadFont(fontPath);
    }
    
    ~Button() {
        if (font != nullptr) {
            TTF_CloseFont(font);
        }
    }
    
    void update(Input& input) override {
        // Get mouse position
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        // Check if mouse is hovering over button
        bool hover = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                     mouseY >= rect.y - dynamicElevation && mouseY <= rect.y - dynamicElevation + rect.h);
        
        // Update visual state
        currentColor = hover ? hoverColor : color;
        bottomColor = {
            static_cast<Uint8>(max(0, currentColor.r - 40)),
            static_cast<Uint8>(max(0, currentColor.g - 40)),
            static_cast<Uint8>(max(0, currentColor.b - 40)),
            currentColor.a
        };
        
        // Handle button press
        if (hover) {
            if (input.getMouseStates()["left"]) {
                if (!isPressed) {
                    isPressed = true;
                    dynamicElevation = 0;
                    callback(); // Call the callback function
                }
            } else {
                isPressed = false;
                dynamicElevation = elevation;
            }
        } else {
            isPressed = false;
            dynamicElevation = elevation;
        }
        
        // Update button rectangles
        topRect.y = originalYPos - dynamicElevation;
        bottomRect.y = originalYPos;
    }
    
    void render(SDL_Renderer* renderer) override {
        if (!visible) return;
        
        // Draw bottom rect first (shadow/3D effect)
        if (dynamicElevation > 0) {
            SDL_SetRenderDrawColor(renderer, bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.a);
            SDL_RenderFillRect(renderer, &bottomRect);
        }
        
        // Draw top button
        SDL_Rect adjustedTopRect = {topRect.x, topRect.y, topRect.w, topRect.h};
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        SDL_RenderFillRect(renderer, &adjustedTopRect);
        
        // Draw border
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderDrawRect(renderer, &adjustedTopRect);
        
        // Render text if font is loaded
        if (font != nullptr) {
            renderText(renderer, adjustedTopRect);
        }
    }

private:
    string text;
    function<void()> callback;
    SDL_Color color;
    SDL_Color hoverColor;
    SDL_Color currentColor;
    SDL_Color bottomColor;
    SDL_Color textColor;
    TTF_Font* font = nullptr;
    int elevation;
    int dynamicElevation;
    int originalYPos;
    int fontSize;
    bool isPressed;
    SDL_Rect topRect;
    SDL_Rect bottomRect;
    
    void loadFont(const string& fontPath) {
        // Initialize SDL_ttf if not already initialized
        if (!TTF_WasInit() && TTF_Init() == -1) {
            cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
            return;
        }
        
        // Load the font
        if (!fontPath.empty()) {
            font = TTF_OpenFont(fontPath.c_str(), fontSize);
            if (font == nullptr) {
                cout << "Failed to load font: " << fontPath << " SDL_ttf Error: " << TTF_GetError() << endl;
                // Try to load a default system font
                font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", fontSize);
            }
        } else {
            // Try to load a default system font
            font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", fontSize);
        }
        
        if (font == nullptr) {
            cout << "Failed to load any font! SDL_ttf Error: " << TTF_GetError() << endl;
        }
    }
    
    void renderText(SDL_Renderer* renderer, const SDL_Rect& buttonRect) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
        if (textSurface == nullptr) {
            cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
            return;
        }
        
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture == nullptr) {
            cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
            SDL_FreeSurface(textSurface);
            return;
        }
        
        // Get text dimensions
        int textWidth = textSurface->w;
        int textHeight = textSurface->h;
        
        // Center text on button
        SDL_Rect renderQuad = {
            buttonRect.x + (buttonRect.w - textWidth) / 2, 
            buttonRect.y + (buttonRect.h - textHeight) / 2,
            textWidth, 
            textHeight
        };
        
        // Render to screen
        SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
        
        // Free resources
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
};

// Label class for displaying text
class Label : public UIElement {
public:
    Label(int x, int y, const string& text, SDL_Color color = {255, 255, 255, 255},
          int fontSize = 20, string fontPath = "")
        : UIElement(x, y, 100, 20),  // Initial size will be updated after font loading
          text(text),
          color(color),
          fontSize(fontSize) {
        
        // Load font
        loadFont(fontPath);
        
        // Update rect size based on text dimensions
        updateTextDimensions();
    }
    
    ~Label() {
        if (font != nullptr) {
            TTF_CloseFont(font);
        }
    }
    
    void render(SDL_Renderer* renderer) override {
        if (!visible || font == nullptr) return;
        
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (textSurface == nullptr) {
            cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
            return;
        }
        
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture == nullptr) {
            cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
            SDL_FreeSurface(textSurface);
            return;
        }
        
        // Render to screen
        SDL_RenderCopy(renderer, textTexture, NULL, &rect);
        
        // Free resources
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    
    // Set new text
    void setText(const string& newText) {
        text = newText;
        updateTextDimensions();
    }

private:
    string text;
    SDL_Color color;
    TTF_Font* font = nullptr;
    int fontSize;
    
    void loadFont(const string& fontPath) {
        // Initialize SDL_ttf if not already initialized
        if (!TTF_WasInit() && TTF_Init() == -1) {
            cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
            return;
        }
        
        // Load the font
        if (!fontPath.empty()) {
            font = TTF_OpenFont(fontPath.c_str(), fontSize);
            if (font == nullptr) {
                cout << "Failed to load font: " << fontPath << " SDL_ttf Error: " << TTF_GetError() << endl;
                font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", fontSize);
            }
        } else {
            font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", fontSize);
        }
        
        if (font == nullptr) {
            cout << "Failed to load any font! SDL_ttf Error: " << TTF_GetError() << endl;
        }
    }
    
    void updateTextDimensions() {
        if (font == nullptr) return;
        
        int width, height;
        if (TTF_SizeText(font, text.c_str(), &width, &height) == 0) {
            rect.w = width;
            rect.h = height;
        }
    }
};

// UI Manager to handle all UI elements
class UIManager {
public:
    UIManager(SDL_Renderer* renderer, int screenWidth, int screenHeight)
        : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight) {
        // Create UI surface for rendering (with alpha support)
        uiSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 
                                         0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        if (uiSurface == nullptr) {
            cout << "UI Surface could not be created! SDL Error: " << SDL_GetError() << endl;
        }
        
        uiTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                      SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);
        if (uiTexture == nullptr) {
            cout << "UI Texture could not be created! SDL Error: " << SDL_GetError() << endl;
        }
        
        // Set texture blend mode to support alpha
        SDL_SetTextureBlendMode(uiTexture, SDL_BLENDMODE_BLEND);
    }
    
    ~UIManager() {
        // Clean up elements
        for (auto element : elements) {
            delete element;
        }
        elements.clear();
        
        // Free surface and texture
        if (uiSurface != nullptr) {
            SDL_FreeSurface(uiSurface);
        }
        if (uiTexture != nullptr) {
            SDL_DestroyTexture(uiTexture);
        }
    }
    
    template<typename T, typename... Args>
    T* addElement(Args&&... args) {
        T* element = new T(std::forward<Args>(args)...);  // Use std:: namespace prefix
        elements.push_back(element);
        return element;
    }
    
    void update(Input& input) {
        for (auto element : elements) {
            element->update(input);
        }
    }
    
    void render() {
        // Set render target to our texture
        SDL_SetRenderTarget(renderer, uiTexture);
        
        // Clear with transparent background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        
        // Render all elements
        for (auto element : elements) {
            element->render(renderer);
        }
        
        // Draw cursor (optional)
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawCircle(renderer, mouseX, mouseY, 10, 1);
        
        // Reset render target to default
        SDL_SetRenderTarget(renderer, nullptr);
        
        // Render the UI texture to the screen
        SDL_RenderCopy(renderer, uiTexture, nullptr, nullptr);
    }

private:
    SDL_Renderer* renderer;
    int screenWidth;
    int screenHeight;
    SDL_Surface* uiSurface = nullptr;
    SDL_Texture* uiTexture = nullptr;
    vector<UIElement*> elements;
    
    // Helper function to draw a circle
    void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius, int width) {
        const int diameter = radius * 2;
        
        int x_pos = radius - 1;
        int y_pos = 0;
        int tx = 1;
        int ty = 1;
        int error = tx - diameter;
        
        while (x_pos >= y_pos) {
            SDL_RenderDrawPoint(renderer, x + x_pos, y - y_pos);
            SDL_RenderDrawPoint(renderer, x + x_pos, y + y_pos);
            SDL_RenderDrawPoint(renderer, x - x_pos, y - y_pos);
            SDL_RenderDrawPoint(renderer, x - x_pos, y + y_pos);
            SDL_RenderDrawPoint(renderer, x + y_pos, y - x_pos);
            SDL_RenderDrawPoint(renderer, x + y_pos, y + x_pos);
            SDL_RenderDrawPoint(renderer, x - y_pos, y - x_pos);
            SDL_RenderDrawPoint(renderer, x - y_pos, y + x_pos);
            
            if (error <= 0) {
                y_pos++;
                error += ty;
                ty += 2;
            }
            
            if (error > 0) {
                x_pos--;
                tx += 2;
                error += tx - diameter;
            }
        }
    }
};