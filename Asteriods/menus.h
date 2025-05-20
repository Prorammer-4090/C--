#include <SDL2/SDL.h>
#include "ui.h"

using namespace std;

class Menus {
private:
    map<string, bool> gameStates;
    UIManager uiManager;

    // Control Menu
    Label* controlsLabel;
    Label* forwardLabel;
    Label* turnRightLabel;
    Label* turnLeftLabel;
    Label* shootLabel;
    Label* pauseLabel;

    // Main menu
    Label* gameName;
    Button* startButton;
    Button* volumeButton;
    Button* controlsButton;
    Button* quitButton;
    Button* highScoreButton;

    // Volume Menu
    Label* volumeLabel;
    Button* volumePlusButton;
    Button* volumeMinusButton;
    Label* volumeAmountLabel; // Min 0 , Max 100

    // Pause Menu
    Button* resumeButton;
    Button* restartButton;
    Button* mainMenuButton;

    // GameOver Menu
    Label* gameOverLabel;
    Button* submitScoreButton; // Navigates to highscore recording
    Button* gameOverMainMenuButton;

    // Highscore Recording Menu
    Label* enterNameLabel;
    Label* playerNameDisplayLabel; // Displays the name being typed
    Button* submitNameButton; // Submits the name and score
    string currentNameInput;

    // for going back to the previous menu
    Button* backButton;

    int SCREENWIDTH;
    int SCREENHEIGHT;

    // Store callbacks
    vector<function<void()>> mainMenuCallbacks;
    vector<function<void()>> pauseMenuCallbacks;
    vector<function<void()>> volumeCallbacks;
    vector<function<void()>> gameOverMenuCallbacks;
    function<void(string)> highscoreSubmitCallback; // For submitting name from highscore recording

public:
    Menus(SDL_Renderer* renderer, int width, int height)
        : uiManager(renderer, width, height) {
        gameStates["mainMenu"] = true;
        gameStates["pauseMenu"] = false;
        gameStates["controls"] = false;
        gameStates["volume"] = false;
        gameStates["highScore"] = false;
        gameStates["gameOver"] = false;
        gameStates["highscoreRecording"] = false; // New state
        SCREENWIDTH = width;
        SCREENHEIGHT = height;

        // Initialize all pointers to nullptr
        controlsLabel = nullptr;
        forwardLabel = nullptr;
        turnRightLabel = nullptr;
        turnLeftLabel = nullptr;
        shootLabel = nullptr;
        pauseLabel = nullptr;
        gameName = nullptr;
        startButton = nullptr;
        volumeButton = nullptr;
        controlsButton = nullptr;
        quitButton = nullptr;
        highScoreButton = nullptr;
        volumeLabel = nullptr;
        volumePlusButton = nullptr;
        volumeMinusButton = nullptr;
        volumeAmountLabel = nullptr;
        resumeButton = nullptr;
        restartButton = nullptr;
        mainMenuButton = nullptr;
        backButton = nullptr;

        // GameOver Menu
        gameOverLabel = nullptr;
        submitScoreButton = nullptr;
        gameOverMainMenuButton = nullptr;

        // Highscore Recording Menu
        enterNameLabel = nullptr;
        playerNameDisplayLabel = nullptr;
        submitNameButton = nullptr;
        currentNameInput = "";
    }

    // Add callback for back button navigation
    function<void(bool fromPauseMenu)> onBackFromControls;
    function<void(bool fromPauseMenu)> onBackFromVolume;
    function<void()> onBackFromHighScore;

    // Set back button handler callbacks
    void setBackHandlers(
        function<void(bool)> controlsBackHandler,
        function<void(bool)> volumeBackHandler,
        function<void()> highscoreBackHandler) {
        
        onBackFromControls = controlsBackHandler;
        onBackFromVolume = volumeBackHandler;
        onBackFromHighScore = highscoreBackHandler;
    }

    // Initialize methods
    void initMainMenu(const vector<function<void()>>& callbacks) {
        setMainMenuCallbacks(callbacks);
        mainMenu(callbacks);
    }

    void initPauseMenu(const vector<function<void()>>& callbacks) {
        setPauseMenuCallbacks(callbacks);
        pauseMenu(callbacks);
    }

    void initVolumeMenu(vector<function<void()>> callbacks) {
        setVolumeCallbacks(callbacks);
        volumeMenu(callbacks);
    }

    void initGameOverMenu(const vector<function<void()>>& callbacks) {
        setGameOverMenuCallbacks(callbacks);
        gameOverMenu(callbacks);
    }

    void initHighscoreRecordingMenu(function<void(string)> submitCallback) {
        setHighscoreRecordingMenuCallbacks(submitCallback);
        highscoreRecordingMenu(submitCallback);
    }

    // Show methods - recreate menus instead of toggling visibility
    void showMainMenu() {
        // Clear all UI elements and recreate main menu
        uiManager.clearElements();

        // Recreate main menu
        mainMenu(mainMenuCallbacks);

        // Render UI
        uiManager.render();
    }

    void showPauseMenu() {
        // Clear all UI elements and recreate pause menu
        uiManager.clearElements();

        // Recreate pause menu
        pauseMenu(pauseMenuCallbacks);

        // Render UI
        uiManager.render();
    }

    void showControlsMenu(bool fromPauseMenu) {
        // Clear all UI elements and recreate controls menu
        uiManager.clearElements();

        // Create controls menu with explicit fromPauseMenu parameter
        controlsMenu(fromPauseMenu);
        
        // Log for debugging
        cout << "Showing controls menu, fromPauseMenu=" << (fromPauseMenu ? "true" : "false") << endl;

        // Render UI
        uiManager.render();
    }

    void showVolumeMenu(bool fromPauseMenu) {
        // Clear all UI elements and recreate volume menu
        uiManager.clearElements();

        // Create volume menu with explicit fromPauseMenu parameter
        volumeMenu(volumeCallbacks, fromPauseMenu);
        
        // Log for debugging
        cout << "Showing volume menu, fromPauseMenu=" << (fromPauseMenu ? "true" : "false") << endl;

        // Render UI
        uiManager.render();
    }

    void showHighscoreMenu(vector<string> highscorelist) {
        // Clear all UI elements and recreate highscore menu
        uiManager.clearElements();

        // Create highscore menu
        highscoreMenu(highscorelist);

        // Render UI
        uiManager.render();
    }

    void showGameOverMenu() {
        uiManager.clearElements();
        gameOverMenu(gameOverMenuCallbacks);
        uiManager.render();
    }

    void showHighscoreRecordingMenu() {
        uiManager.clearElements();
        highscoreRecordingMenu(highscoreSubmitCallback);
        // Update the display label with the current (possibly empty) name
        if (playerNameDisplayLabel) {
            playerNameDisplayLabel->setText(currentNameInput);
        }
        uiManager.render();
    }

    // Store callbacks for recreating menus
    void setMainMenuCallbacks(const vector<function<void()>>& callbacks) {
        mainMenuCallbacks = callbacks;
    }

    void setPauseMenuCallbacks(const vector<function<void()>>& callbacks) {
        pauseMenuCallbacks = callbacks;
    }

    void setVolumeCallbacks(const vector<function<void()>>& callbacks) {
        volumeCallbacks = callbacks;
    }

    void setGameOverMenuCallbacks(const vector<function<void()>>& callbacks) {
        gameOverMenuCallbacks = callbacks;
    }

    void setHighscoreRecordingMenuCallbacks(function<void(string)> submitCallback) {
        highscoreSubmitCallback = submitCallback;
    }

    // Update the UI manager with input
    void update(Input& input) {
        uiManager.update(input);
    }

    // Methods for handling name input
    void handleTextInput(const string& key) {
        if (key == "Backspace") {
            if (!currentNameInput.empty()) {
                currentNameInput.pop_back();
            }
        } else if (key == "Space") {
            currentNameInput += " ";
        } else if (key.length() == 1 && currentNameInput.length() < 20) { // Limit name length
            currentNameInput += key;
        }
        if (playerNameDisplayLabel) {
            playerNameDisplayLabel->setText(currentNameInput);
        }
    }

    string getCurrentNameInput() const {
        return currentNameInput;
    }

    void clearCurrentNameInput() {
        currentNameInput = "";
        if (playerNameDisplayLabel) {
            playerNameDisplayLabel->setText(currentNameInput);
        }
    }

private:
    void mainMenu(const vector<function<void()>>& callbacks) {
        gameName = uiManager.addElement<Label>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 150,
            "Asteroids",
            SDL_Color{255, 255, 255, 255},
            50,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        startButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2, 250, 50,
            "Start",
            callbacks[0],
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        controlsButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 50, 250, 50,
            "Controls",
            callbacks[1],
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        volumeButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 100, 250, 50,
            "Volume",
            callbacks[2],
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        highScoreButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 150, 250, 50,
            "Highscore",
            callbacks[3],
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        quitButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 200, 250, 50,
            "Quit",
            callbacks[4],
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );
    }

    void controlsMenu(bool fromPauseMenu = false) {
        controlsLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, 100,
            "Controls",
            SDL_Color{255, 255, 255, 255},
            50,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        // Create labels for each control with key binding information
        forwardLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, 200,
            "Forward: UP Arrow / W",
            SDL_Color{255, 255, 255, 255},
            30,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        turnRightLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, 250,
            "Turn Right: RIGHT Arrow / D",
            SDL_Color{255, 255, 255, 255},
            30,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        turnLeftLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, 300,
            "Turn Left: LEFT Arrow / A",
            SDL_Color{255, 255, 255, 255},
            30,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        shootLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, 350,
            "Shoot: SPACE",
            SDL_Color{255, 255, 255, 255},
            30,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        pauseLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, 400,
            "Pause: ESC / P",
            SDL_Color{255, 255, 255, 255},
            30,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        // Back button with external callback
        backButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT - 250, 200, 50,
            "Back",
            [this, fromPauseMenu]() {
                // Update internal state
                gameStates["controls"] = false;
                if (fromPauseMenu) {
                    gameStates["pauseMenu"] = true;
                } else {
                    gameStates["mainMenu"] = true;
                }
                
                // Call external callback if set
                if (onBackFromControls) {
                    onBackFromControls(fromPauseMenu);
                }
            },
            SDL_Color{200, 50, 50, 255},
            SDL_Color{255, 100, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );
    }

    void volumeMenu(vector<function<void()>> callbacks, bool fromPauseMenu = false) {
        volumeLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 150,
            "Volume Settings",
            SDL_Color{255, 255, 255, 255},
            40,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        volumeAmountLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 50,
            "50", // Default volume value
            SDL_Color{255, 255, 255, 255},
            30,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        volumePlusButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2 + 100, SCREENHEIGHT / 2 - 50, 60, 40,
            "+",
            callbacks[0], // Increase volume callback
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        volumeMinusButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2 - 100, SCREENHEIGHT / 2 - 50, 60, 40,
            "-",
            callbacks[1], // Decrease volume callback
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        backButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 100, 200, 50,
            "Back",
            [this, fromPauseMenu]() {
                // Update internal state
                gameStates["volume"] = false;
                if (fromPauseMenu) {
                    gameStates["pauseMenu"] = true;
                } else {
                    gameStates["mainMenu"] = true;
                }
                
                // Call external callback if set
                if (onBackFromVolume) {
                    onBackFromVolume(fromPauseMenu);
                }
            },
            SDL_Color{200, 50, 50, 255},
            SDL_Color{255, 100, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );
    }

    void pauseMenu(const vector<function<void()>>& callbacks) {
        Label* pauseTitle = uiManager.addElement<Label>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 200,
            "Game Paused",
            SDL_Color{255, 255, 255, 255},
            50,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        resumeButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 100, 250, 50,
            "Resume",
            callbacks[0], // Resume game callback
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        restartButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2, 250, 50,
            "Restart",
            callbacks[1], // Restart game callback
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        controlsButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 100, 250, 50,
            "Controls",
            callbacks[2], // Show controls callback
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        volumeButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 200, 250, 50,
            "Volume",
            callbacks[3],
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        mainMenuButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 300, 250, 50,
            "Main Menu",
            callbacks[4], // Return to main menu callback
            SDL_Color{200, 50, 50, 255},
            SDL_Color{255, 100, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );
    }

    void highscoreMenu(vector<string> highscorelist) {
        Label* highscoreTitle = uiManager.addElement<Label>(
            SCREENWIDTH / 2, 100,
            "High Scores",
            SDL_Color{255, 255, 255, 255},
            50,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        // Display up to 10 high scores
        int maxScores = min(10, static_cast<int>(highscorelist.size()));
        for (int i = 0; i < maxScores; i++) {
            uiManager.addElement<Label>(
                SCREENWIDTH / 2, 180 + i * 50,
                highscorelist[i],
                SDL_Color{255, 255, 255, 255},
                25,
                "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");
        }

        backButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT - 100, 200, 50,
            "Back",
            [this]() { 
                // Update internal state
                gameStates["highScore"] = false; 
                gameStates["mainMenu"] = true;
                
                // Call external callback if set
                if (onBackFromHighScore) {
                    onBackFromHighScore();
                }
            },
            SDL_Color{200, 50, 50, 255},
            SDL_Color{255, 100, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );
    }

    void gameOverMenu(const vector<function<void()>>& callbacks) {
        gameOverLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 150,
            "Game Over",
            SDL_Color{255, 0, 0, 255}, // Red color for game over
            60,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        submitScoreButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2, 300, 50,
            "Submit Score",
            callbacks[0], // Callback to go to highscore recording screen
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );

        gameOverMainMenuButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 75, 300, 50,
            "Main Menu",
            callbacks[1], // Callback to go to main menu
            SDL_Color{200, 50, 50, 255},
            SDL_Color{255, 100, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );
    }

    void highscoreRecordingMenu(function<void(string)> submitCallback) {
        enterNameLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 100,
            "Enter Your Name:",
            SDL_Color{255, 255, 255, 255},
            40,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        playerNameDisplayLabel = uiManager.addElement<Label>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 - 30,
            currentNameInput, // Display current name input
            SDL_Color{255, 255, 0, 255}, // Yellow for input text
            30,
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf");

        submitNameButton = uiManager.addElement<Button>(
            SCREENWIDTH / 2, SCREENHEIGHT / 2 + 50, 250, 50,
            "Submit",
            [this, submitCallback]() {
                if (submitCallback) {
                    submitCallback(currentNameInput);
                }
            },
            SDL_Color{50, 200, 50, 255},
            SDL_Color{100, 255, 100, 255},
            "/Users/jethroaiyesan/Programming/Tutorials/C++/fonts/Silkscreen-Regular.ttf"
        );
    }
};