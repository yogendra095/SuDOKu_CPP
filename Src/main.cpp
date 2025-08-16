#include <SFML/Graphics.hpp>
#include "User.hpp"
#include <iostream>
#include <string>
#include "menu.hpp"
#include <memory>
#include "LevelSelection.hpp"
#include "SudokuGame.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

enum class LoginState {
    MENU,
    NEW_USER_NAME,
    EXISTING_USER_ID,
    LOGGED_IN,
    PLAYING_SUDOKU,
    VIEW_RANKING
};
std::unique_ptr<SudokuGame> currentSudoku; 
int currentLevel = 1; 

struct RankingEntry {
    std::string username;
    int score;
};

std::vector<RankingEntry> getRankings() {
    std::vector<RankingEntry> rankings;
    std::ifstream file("users.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id, username;
        int score, diamonds, unlockedLevel;
        if (iss >> id >> username >> score >> diamonds >> unlockedLevel) {
            rankings.push_back({username, score});
        }
    }
    std::sort(rankings.begin(), rankings.end(), [](const RankingEntry& a, const RankingEntry& b) {
        return a.score > b.score;
    });
    return rankings;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Sudoku Login", sf::Style::Default | sf::Style::Resize);

    // Load the background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets\\images\\background.jpeg")) {
    return -1;  // Couldn't load image
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // Fit background to window
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();
    float scaleX = float(windowSize.x) / textureSize.x;
    float scaleY = float(windowSize.y) / textureSize.y;
    float scale = std::max(scaleX, scaleY);
    backgroundSprite.setScale(sf::Vector2f(scale, scale));
    backgroundSprite.setPosition(sf::Vector2f(
        (windowSize.x - textureSize.x * scale) / 2.f,
        (windowSize.y - textureSize.y * scale) / 2.f
    ));

    // Load the font
    sf::Font font;
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    // Main title
    sf::Text titleText(font);
    titleText.setString("Sudoku Game - Login");
    titleText.setCharacterSize(54); // Big title
    titleText.setFillColor(sf::Color(255, 255, 255));
    titleText.setOutlineColor(sf::Color(60, 30, 0));
    titleText.setOutlineThickness(5); // Bold outline
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);

    // Center the title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x / 2.0f,
        titleBounds.position.y
    ));
    titleText.setPosition(sf::Vector2f(windowSize.x / 2.0f, 40.f));

    // Add a shadow to the title
    sf::Text titleShadow = titleText;
    titleShadow.setFillColor(sf::Color(0, 0, 0, 120)); // Shadow color
    titleShadow.setOutlineThickness(0);
    titleShadow.move(sf::Vector2f(3.f, 3.f)); // Move shadow

    // Title after login
    sf::Text gameTitleText(font);
    gameTitleText.setString("SuDoKu");
    gameTitleText.setCharacterSize(56);
    gameTitleText.setFillColor(sf::Color(255, 255, 255));
    gameTitleText.setOutlineColor(sf::Color(60, 30, 0));
    gameTitleText.setOutlineThickness(4);
    gameTitleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    gameTitleText.setPosition({windowSize.x / 2.f - 120, 40});

    // Menu buttons
    sf::Text newUserOption(font);
    newUserOption.setString("New User");
    newUserOption.setCharacterSize(32);
    newUserOption.setFillColor(sf::Color(40, 40, 60)); // Button color
    newUserOption.setOutlineColor(sf::Color(255, 255, 255, 180)); // Button outline
    newUserOption.setOutlineThickness(2);
    newUserOption.setStyle(sf::Text::Bold);
    newUserOption.setPosition({windowSize.x / 2.f - 100, 150});

    sf::Text existingUserOption(font);
    existingUserOption.setString("Existing User");
    existingUserOption.setCharacterSize(32);
    existingUserOption.setFillColor(sf::Color(40, 40, 60)); // Button color
    existingUserOption.setOutlineColor(sf::Color(255, 255, 255, 180)); // Button outline
    existingUserOption.setOutlineThickness(2);
    existingUserOption.setStyle(sf::Text::Bold);
    existingUserOption.setPosition({windowSize.x / 2.f - 100, 200});

    sf::Text menuPrompt(font);
    menuPrompt.setString("Press 1 or 2 to select:");
    menuPrompt.setCharacterSize(22);
    menuPrompt.setFillColor(sf::Color(255, 255, 180));
    menuPrompt.setOutlineColor(sf::Color(60, 30, 0));
    menuPrompt.setOutlineThickness(2);
    menuPrompt.setPosition({windowSize.x / 2.f - 120, 250});

    // Prompt for input
    sf::Text inputPrompt(font);
    inputPrompt.setCharacterSize(26);
    inputPrompt.setFillColor(sf::Color(255, 255, 255));
    inputPrompt.setOutlineColor(sf::Color(60, 30, 0));
    inputPrompt.setOutlineThickness(2);
    inputPrompt.setPosition({100, 300});

    // Show what user types
    sf::Text inputDisplay(font);
    inputDisplay.setString("");
    inputDisplay.setCharacterSize(26);
    inputDisplay.setFillColor(sf::Color(255, 255, 180));
    inputDisplay.setOutlineColor(sf::Color(60, 30, 0));
    inputDisplay.setOutlineThickness(2);
    inputDisplay.setPosition({100, 350});

    // Show status
    sf::Text statusMessage(font);
    statusMessage.setString("");
    statusMessage.setCharacterSize(22);
    statusMessage.setFillColor(sf::Color(0, 255, 0));
    statusMessage.setOutlineColor(sf::Color(60, 30, 0));
    statusMessage.setOutlineThickness(2);
    statusMessage.setPosition({100, 450});

    // Show instructions
    sf::Text instructions(font);
    instructions.setString("Press ESC to go back | Enter to confirm");
    instructions.setCharacterSize(18);
    instructions.setFillColor(sf::Color(255, 255, 180));
    instructions.setOutlineColor(sf::Color(60, 30, 0));
    instructions.setOutlineThickness(2);
    instructions.setPosition({windowSize.x / 2.f - 180, 550});

    // Timer text
    sf::Text timerText(font);
    timerText.setCharacterSize(26);
    timerText.setFillColor(sf::Color(255, 80, 80));
    timerText.setOutlineColor(sf::Color(60, 30, 0));
    timerText.setOutlineThickness(2);
    timerText.setPosition(sf::Vector2f(static_cast<float>(windowSize.x) - 180.f, 20.f));

    // Score text
    sf::Text scoreText(font);
    scoreText.setCharacterSize(26);
    scoreText.setFillColor(sf::Color(80, 255, 80));
    scoreText.setOutlineColor(sf::Color(60, 30, 0));
    scoreText.setOutlineThickness(2);
    scoreText.setPosition(sf::Vector2f(windowSize.x / 2.f, 20.f));

    // Diamonds text
    sf::Text diamondsText(font);
    diamondsText.setCharacterSize(26);
    diamondsText.setFillColor(sf::Color(200, 0, 200));
    diamondsText.setOutlineColor(sf::Color(60, 30, 0));
    diamondsText.setOutlineThickness(2);
    diamondsText.setStyle(sf::Text::Bold);
    diamondsText.setPosition(sf::Vector2f(10.f, 20.f));

    // Game state variables
    LoginState currentState = LoginState::MENU;
    std::string userInput = "";
    User user;
    bool showInstructions = true;
    std::unique_ptr<Menu> gameMenu;
    bool menuInitialized = false;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // Handle window resize
            else if (const auto* resize = event->getIf<sf::Event::Resized>()) {
                windowSize = {resize->size.x, resize->size.y};

                // Rescale and reposition background
                float scaleX = float(windowSize.x) / textureSize.x;
                float scaleY = float(windowSize.y) / textureSize.y;
                float scale = std::max(scaleX, scaleY);
                backgroundSprite.setScale(sf::Vector2f(scale, scale));
                backgroundSprite.setPosition(sf::Vector2f(
                    (windowSize.x - textureSize.x * scale) / 2.f,
                    (windowSize.y - textureSize.y * scale) / 2.f
                ));

                // Optionally, update view to match new size
                window.setView(sf::View(sf::FloatRect({0.f, 0.f}, sf::Vector2f(windowSize.x, windowSize.y))));
            }
            // ESC key handling
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    if (currentState == LoginState::MENU) {
                        window.close();
                    } 
                    else if (currentState == LoginState::PLAYING_SUDOKU) {
                        LevelSelection levelSelection(font, user.getUnlockedLevel());
                        if (levelSelection.run(window)) {
                            currentLevel = levelSelection.getSelectedLevel();
                            int difficulty = 1;
                            if(currentLevel >= 3 && currentLevel <= 4) difficulty = 2;
                            else if(currentLevel >= 5 && currentLevel <= 6) difficulty = 3;
                            else if(currentLevel >= 7) difficulty = 4;
                            currentSudoku = std::make_unique<SudokuGame>(font, difficulty, user.getDiamonds());
                            currentState = LoginState::PLAYING_SUDOKU;
                        } else {
                            currentState = LoginState::LOGGED_IN;
                        }
                    }
                        else if (currentState == LoginState::VIEW_RANKING) {
                            currentState = LoginState::LOGGED_IN;
                            userInput = "";
                            inputDisplay.setString("");
                            statusMessage.setString("");
                            showInstructions = true;
                        }
                    else if (currentState != LoginState::LOGGED_IN) {
                        currentState = LoginState::MENU;
                        userInput = "";
                        inputDisplay.setString("");
                        statusMessage.setString("");
                        showInstructions = true;
                    }
                }
                // Arrow keys for Sudoku grid navigation
                if (currentState == LoginState::PLAYING_SUDOKU) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::Left:
                        case sf::Keyboard::Key::Right:
                            if (currentSudoku) currentSudoku->handleArrowKeys(keyPressed->code);
                            break;
                        default:
                            break;
                    }
                }
                // Number keys for Sudoku input
                if (currentState == LoginState::PLAYING_SUDOKU) {
                    int num = 0;
                    switch(keyPressed->code) {
                        case sf::Keyboard::Key::Num1: num = 1; break;
                        case sf::Keyboard::Key::Num2: num = 2; break;
                        case sf::Keyboard::Key::Num3: num = 3; break;
                        case sf::Keyboard::Key::Num4: num = 4; break;
                        case sf::Keyboard::Key::Num5: num = 5; break;
                        case sf::Keyboard::Key::Num6: num = 6; break;
                        case sf::Keyboard::Key::Num7: num = 7; break;
                        case sf::Keyboard::Key::Num8: num = 8; break;
                        case sf::Keyboard::Key::Num9: num = 9; break;
                        default: num = 0; break;
                    }
                    if(num > 0 && currentSudoku) {
                        currentSudoku->handleKeyboardInput(num, user);
                    }
                }
            }

            // Text input for name entry
            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                if (currentState == LoginState::NEW_USER_NAME || currentState == LoginState::EXISTING_USER_ID) {
                    char inputChar = static_cast<char>(textEntered->unicode);
                    if (textEntered->unicode == '\b' && !userInput.empty()) {
                        userInput.pop_back();
                        inputDisplay.setString(userInput);
                    }
                    else if (textEntered->unicode == '\r' || textEntered->unicode == '\n') {
                        if (!userInput.empty()) {
                            if (currentState == LoginState::NEW_USER_NAME) {
                                // Check if username already exists (case-sensitive)
                                std::ifstream file("users.txt");
                                std::string line, existingId, existingName;
                                bool exists = false;
                                while (std::getline(file, line)) {
                                    std::istringstream iss(line);
                                    iss >> existingId >> existingName;
                                    if (userInput == existingName) {
                                        exists = true;
                                        break;
                                    }
                                }
                                if (exists) {
                                    statusMessage.setString("Username already exists! Please choose another name.");
                                    statusMessage.setFillColor(sf::Color::Red);
                                    userInput = "";
                                    inputDisplay.setString("");
                                } else {
                                    user.createNew(userInput);
                                    statusMessage.setString("Welcome " + userInput + "! Your ID is: " + user.getId());
                                    statusMessage.setFillColor(sf::Color::Green);
                                    currentState = LoginState::LOGGED_IN;
                                    showInstructions = false;
                                }
                            }
                            else if (currentState == LoginState::EXISTING_USER_ID) {
                                if (user.login(userInput)) {
                                    statusMessage.setString("Welcome back, " + user.getName() + "! Your ID: " + user.getId());
                                    statusMessage.setFillColor(sf::Color::Green);
                                    currentState = LoginState::LOGGED_IN;
                                    showInstructions = false;
                                } else {
                                    statusMessage.setString("User not found! Please check your name and try again.");
                                    statusMessage.setFillColor(sf::Color::Red);
                                    userInput = "";
                                    inputDisplay.setString("");
                                }
                            }
                        }
                    }
                    else if (textEntered->unicode >= 32 && textEntered->unicode < 127) {
                        userInput += inputChar;
                        inputDisplay.setString(userInput);
                    }
                }
            }

            // Mouse click for menu and Sudoku grid
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords({mousePressed->position.x, mousePressed->position.y});
                    if (currentState == LoginState::MENU) {
                        if (newUserOption.getGlobalBounds().contains(mousePos)) {
                            currentState = LoginState::NEW_USER_NAME;
                            inputPrompt.setString("Enter your name:");
                            userInput = "";
                            statusMessage.setString("");
                            showInstructions = true;
                        } else if (existingUserOption.getGlobalBounds().contains(mousePos)) {
                            currentState = LoginState::EXISTING_USER_ID;
                            inputPrompt.setString("Enter your name:");
                            userInput = "";
                            statusMessage.setString("");
                            showInstructions = true;
                        }
                    }
                    else if (currentState == LoginState::PLAYING_SUDOKU && currentSudoku) {
                        currentSudoku->handleMouseClick(mousePos);
                    }
                }
            }

            // Menu navigation with mouse for LOGGED_IN state
            else if (currentState == LoginState::LOGGED_IN && gameMenu) {
                gameMenu->handleEvent(*event, window);
            }
        }
        
        static sf::Clock endMessageClock;
        static std::string endMessage = "";
        static bool showEndMessage = false;
    static LoginState nextStateAfterMessage = LoginState::LOGGED_IN;
        if (currentState == LoginState::PLAYING_SUDOKU) {
            if (currentSudoku) {
                if (currentSudoku->isCompleted()) {
                    int reward = currentLevel; //Diamond
                    // New scoring system
                    int difficulty = 1;
                    if(currentLevel >= 3 && currentLevel <= 4) difficulty = 2;
                    else if(currentLevel >= 5 && currentLevel <= 6) difficulty = 3;
                    else if(currentLevel >= 7) difficulty = 4;
                    int baseScore = 100 * difficulty;
                    int maxTime = 0;
                    switch(difficulty) {
                        case 1: maxTime = 20 * 60; break;
                        case 2: maxTime = 30 * 60; break;
                        case 3: maxTime = 40 * 60; break;
                        case 4: maxTime = 50 * 60; break;
                        default: maxTime = 5 * 60; break;
                    }
                    int timeTaken = maxTime - currentSudoku->getRemainingTime();
                    int timeBonus = std::max(0, (maxTime - timeTaken) * 2);
                    int mistakePenalty = currentSudoku->getMistakeCount() * 10;
                    int finalScore = baseScore + timeBonus - mistakePenalty;
                    if (finalScore < 0) finalScore = 0;
                    user.addDiamonds(reward);
                    user.addScore(finalScore);
                    if (currentLevel == user.getUnlockedLevel() && currentLevel < 8) {
                        user.setUnlockedLevel(currentLevel + 1);
                    }
                    user.save();
                    currentSudoku.reset();
                    endMessage = "Level Completed!";
                    showEndMessage = true;
                    endMessageClock.restart();
                    nextStateAfterMessage = LoginState::LOGGED_IN; // Will trigger level selection below
                    currentState = LoginState::PLAYING_SUDOKU; // Stay in this state to show message
                }
                else if (currentSudoku->getRemainingTime() <= 0) {
                    user.addDiamonds(currentSudoku->getDiamonds());
                    user.save();
                    currentSudoku.reset();
                    endMessage = "Time Over!";
                    showEndMessage = true;
                    endMessageClock.restart();
                    nextStateAfterMessage = LoginState::LOGGED_IN;
                    currentState = LoginState::PLAYING_SUDOKU;
                }
                else if(currentSudoku->getDiamonds() == 0 && currentSudoku->gameOver){
                    currentSudoku.reset();
                    endMessage = "Game Over! No Diamonds Left.";
                    showEndMessage = true;
                    endMessageClock.restart();
                    nextStateAfterMessage = LoginState::LOGGED_IN;
                    currentState = LoginState::PLAYING_SUDOKU;
                }
            }
        }
        // Show end message and after 2 seconds go to level selection
        if (showEndMessage) {
            if (endMessageClock.getElapsedTime().asSeconds() > 2.0f) {
                showEndMessage = false;
                // Instead of just LOGGED_IN, go to level selection
                LevelSelection levelSelection(font, user.getUnlockedLevel());
                if (levelSelection.run(window)) {
                    currentLevel = levelSelection.getSelectedLevel();
                    int difficulty = 1;
                    if(currentLevel >= 3 && currentLevel <= 4) difficulty = 2;
                    else if(currentLevel >= 5 && currentLevel <= 6) difficulty = 3;
                    else if(currentLevel >= 7) difficulty = 4;
                    currentSudoku = std::make_unique<SudokuGame>(font, difficulty, user.getDiamonds());
                    currentState = LoginState::PLAYING_SUDOKU;
                } else {
                    currentState = LoginState::LOGGED_IN;
                }
            }
        }

        // Clear and draw
        window.clear(sf::Color::White);

        // Draw the background image
        window.draw(backgroundSprite);

        // Only draw Title on menu and login screens
        if (currentState == LoginState::MENU ||
            currentState == LoginState::NEW_USER_NAME ||
            currentState == LoginState::EXISTING_USER_ID) {
            window.draw(titleShadow); // Draw shadow first
            window.draw(titleText);   // Then draw main title
        }

        // IF successfully login
        if (currentState == LoginState::LOGGED_IN) {
            window.draw(gameTitleText);
        }

        // Draw based on current state
        switch (currentState) {
            case LoginState::MENU: {
    // Draw menu box
    float boxWidth = 350.f;
    float boxHeight = 180.f;
    float boxX = windowSize.x / 2.f - boxWidth / 2.f;
    float boxY = windowSize.y / 2.f - boxHeight / 2.f;

    sf::RectangleShape menuBox(sf::Vector2f(boxWidth, boxHeight));
    menuBox.setPosition(sf::Vector2f(boxX, boxY));
    menuBox.setFillColor(sf::Color(255, 255, 255, 220)); // semi-transparent white
    menuBox.setOutlineColor(sf::Color(60, 30, 0));
    menuBox.setOutlineThickness(4.f);

    window.draw(menuBox);

    // Center the options inside the box
    sf::FloatRect newUserBounds = newUserOption.getLocalBounds();
    sf::FloatRect existingUserBounds = existingUserOption.getLocalBounds();

    newUserOption.setPosition(sf::Vector2f(
        boxX + boxWidth / 2.f - newUserBounds.size.x / 2.f,
        boxY + 40.f
    ));
    existingUserOption.setPosition(sf::Vector2f(
        boxX + boxWidth / 2.f - existingUserBounds.size.x / 2.f,
        boxY + 100.f
    ));

    // Mouse hover effects (keep this logic)
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    if (newUserOption.getGlobalBounds().contains(mousePos)) {
        newUserOption.setFillColor(sf::Color(255, 140, 0)); // Vibrant orange
        newUserOption.setOutlineColor(sf::Color(60, 30, 0));
    } else {
        newUserOption.setFillColor(sf::Color(40, 40, 60)); // Darker text
        newUserOption.setOutlineColor(sf::Color(255, 255, 255, 180));
    }
    if (existingUserOption.getGlobalBounds().contains(mousePos)) {
        existingUserOption.setFillColor(sf::Color(255, 140, 0));
        existingUserOption.setOutlineColor(sf::Color(60, 30, 0));
    } else {
        existingUserOption.setFillColor(sf::Color(40, 40, 60));
        existingUserOption.setOutlineColor(sf::Color(255, 255, 255, 180));
    }

    window.draw(newUserOption);
    window.draw(existingUserOption);
    break;
}
    case LoginState::NEW_USER_NAME:
    case LoginState::EXISTING_USER_ID: {
        // Draw a semi-transparent, rounded rectangle box (use RectangleShape for simplicity)
        float boxWidth = 420.f;
        float boxHeight = 180.f;
        float boxX = windowSize.x / 2.f - boxWidth / 2.f;
        float boxY = windowSize.y / 2.f - boxHeight / 2.f;

        sf::RectangleShape inputBox(sf::Vector2f(boxWidth, boxHeight));
        inputBox.setPosition(sf::Vector2f(boxX, boxY));
        inputBox.setFillColor(sf::Color(255, 255, 255, 230)); // Soft white
        inputBox.setOutlineColor(sf::Color(60, 30, 0));
        inputBox.setOutlineThickness(4.f);

        // Optional: Add a shadow
        sf::RectangleShape inputBoxShadow = inputBox;
        inputBoxShadow.setFillColor(sf::Color(0, 0, 0, 60));
        inputBoxShadow.move(sf::Vector2f(5.f, 5.f));
        window.draw(inputBoxShadow);

        window.draw(inputBox);

        // Prompt text (centered, bold, friendly color)
        sf::Text prompt(inputPrompt);
        prompt.setCharacterSize(32);
        prompt.setFillColor(sf::Color(255, 140, 0));
        prompt.setOutlineColor(sf::Color(60, 30, 0));
        prompt.setOutlineThickness(2);
        prompt.setStyle(sf::Text::Bold);
        sf::FloatRect promptBounds = prompt.getLocalBounds();
        prompt.setOrigin(sf::Vector2f(promptBounds.size.x / 2.f, 0));
        prompt.setPosition(sf::Vector2f(windowSize.x / 2.f, boxY + 30.f));
        window.draw(prompt);

        // Input display (centered, under prompt)
        sf::Text input(inputDisplay);
        input.setCharacterSize(28);
        input.setFillColor(sf::Color(40, 40, 60));
        input.setOutlineColor(sf::Color(255, 140, 0));
        input.setOutlineThickness(2);
        sf::FloatRect inputBounds = input.getLocalBounds();
        input.setOrigin(sf::Vector2f(inputBounds.size.x / 2.f, 0));
        input.setPosition(sf::Vector2f(windowSize.x / 2.f, boxY + 80.f));
        window.draw(input);

        // Status message (centered, below input)
        sf::Text status(statusMessage);
        status.setCharacterSize(20);
        status.setOutlineThickness(1);
        sf::FloatRect statusBounds = status.getLocalBounds();
        status.setOrigin(sf::Vector2f(statusBounds.size.x / 2.f, 0));
        status.setPosition(sf::Vector2f(windowSize.x / 2.f, boxY + 130.f));
        window.draw(status);

        // Instructions (centered, below box)
        if (showInstructions) {
            sf::Text instr(instructions);
            instr.setCharacterSize(16);
            instr.setFillColor(sf::Color(120, 120, 120));
            sf::FloatRect instrBounds = instr.getLocalBounds();
            instr.setOrigin(sf::Vector2f(instrBounds.size.x / 2.f, 0));
            instr.setPosition(sf::Vector2f(windowSize.x / 2.f, boxY + boxHeight + 10.f));
            window.draw(instr);
        }
        break;
    }
    case LoginState::LOGGED_IN: {
        // Initialize menu once
        if (!menuInitialized) {
            gameMenu = std::make_unique<Menu>(font);
            gameMenu->setPosition(windowSize.x / 2.f - 100, 200);
            gameMenu->addButton("Play Sudoku", [&](){
                LevelSelection levelSelection(font, user.getUnlockedLevel());
                if(levelSelection.run(window)) {
                    currentLevel = levelSelection.getSelectedLevel();
                    int difficulty = 1;
                    if(currentLevel >= 3 && currentLevel <= 4) difficulty = 2;
                    else if(currentLevel >= 5 && currentLevel <= 6) difficulty = 3;
                    else if(currentLevel >= 7) difficulty = 4;
                    currentSudoku = std::make_unique<SudokuGame>(font, difficulty, user.getDiamonds());
                    currentState = LoginState::PLAYING_SUDOKU;
                }
            });
            gameMenu->addButton("View Ranking", [&](){
                currentState = LoginState::VIEW_RANKING;
            });
            gameMenu->addButton("Exit", [&]{ 
                currentState = LoginState::MENU;
                userInput = "";
                inputDisplay.setString("");
                statusMessage.setString("");
                showInstructions = true;
                menuInitialized = false;
            });
            menuInitialized = true;
        }

        // Draw menu (replaces the success text)
        gameMenu->draw(window);
        window.draw(statusMessage); // Keep your existing status message
        break;
    }
    case LoginState::PLAYING_SUDOKU: {
        if (currentSudoku) {
            currentSudoku->draw(window, user);
            // All top bar info (diamonds, score, time) is now handled inside SudokuGame::draw
        }
        // Draw end message overlay if needed
        if (showEndMessage) {
            sf::RectangleShape overlay(sf::Vector2f(windowSize.x, windowSize.y));
            overlay.setFillColor(sf::Color(0,0,0,120));
            window.draw(overlay);
            sf::Text msgText(font, endMessage, 48);
            msgText.setFillColor(sf::Color(255, 215, 0));
            msgText.setOutlineColor(sf::Color(60, 30, 0));
            msgText.setOutlineThickness(4);
            sf::FloatRect msgBounds = msgText.getLocalBounds();
            msgText.setOrigin({msgBounds.size.x/2.f, msgBounds.size.y/2.f});
            msgText.setPosition({windowSize.x/2.f, windowSize.y/2.f});
            window.draw(msgText);
        }
        break;
    }
    case LoginState::VIEW_RANKING: {
        // Draw background (use your wood texture if available)
        window.draw(backgroundSprite);

        // Title with wood-style box
        sf::RectangleShape titleBox(sf::Vector2f(400.f, 60.f));
        titleBox.setFillColor(sf::Color(181, 140, 90));
        titleBox.setOutlineColor(sf::Color(120, 80, 40));
        titleBox.setOutlineThickness(4.f);
        titleBox.setPosition({windowSize.x / 2.f - 200.f, 30.f});
        window.draw(titleBox);

        sf::Text rankingTitle(font);
        rankingTitle.setString("LEADERBOARD");
        rankingTitle.setCharacterSize(36);
        rankingTitle.setFillColor(sf::Color(60, 30, 0));
        rankingTitle.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = rankingTitle.getLocalBounds();
        rankingTitle.setOrigin({titleBounds.size.x / 2.f, titleBounds.size.y / 2.f});
        rankingTitle.setPosition({windowSize.x / 2.f, 60.f});
        window.draw(rankingTitle);

        // Rankings
        auto rankings = getRankings();
        float entryBoxWidth = 600.f;
        float entryBoxHeight = 48.f;
        float startY = 120.f;
        int rank = 1;

        // Draw column headers above the entries
        sf::Text header(font);
        header.setCharacterSize(26);
        header.setFillColor(sf::Color(120, 80, 40));
        header.setStyle(sf::Text::Bold);
        header.setPosition({windowSize.x / 2.f - 250.f, startY - 40.f});
        window.draw(header);

        // Calculate X positions for columns based on header
        float rankColX = windowSize.x / 2.f - 250.f + 10.f;
        float nameColX = windowSize.x / 2.f - 250.f + 90.f;
        float scoreColX = windowSize.x / 2.f + 250.f - 80.f;

        sf::Text rankText(font), nameText(font), scoreText(font);
        rankText.setCharacterSize(24);
        nameText.setCharacterSize(24);
        scoreText.setCharacterSize(24);

        for (const auto& r : rankings) {
            float y = startY + (rank - 1) * (entryBoxHeight + 12.f);

            // Draw plank/box for entry
            sf::RectangleShape entryBox(sf::Vector2f(entryBoxWidth, entryBoxHeight));
            entryBox.setPosition({windowSize.x / 2.f - entryBoxWidth / 2.f, y});
            entryBox.setFillColor(sf::Color(222, 184, 135, 230));
            entryBox.setOutlineColor(sf::Color(120, 80, 40));
            entryBox.setOutlineThickness(3.f);
            window.draw(entryBox);

            // Draw rank icon/circle
            sf::CircleShape rankCircle(18.f);
            rankCircle.setPosition({rankColX - 30.f, y + 6.f});
            if (rank == 1)
                rankCircle.setFillColor(sf::Color(255, 215, 0)); // Gold
            else if (rank == 2)
                rankCircle.setFillColor(sf::Color(192, 192, 192)); // Silver
            else if (rank == 3)
                rankCircle.setFillColor(sf::Color(205, 127, 50)); // Bronze
            else
                rankCircle.setFillColor(sf::Color(200, 200, 200));
            window.draw(rankCircle);

            // Rank number (centered in circle)
            rankText.setString(std::to_string(rank));
            rankText.setFillColor(sf::Color(60, 30, 0));
            rankText.setStyle(sf::Text::Bold);
            sf::FloatRect rankBounds = rankText.getLocalBounds();
            rankText.setOrigin({rankBounds.size.x / 2.f, rankBounds.size.y / 2.f});
            rankText.setPosition({rankCircle.getPosition().x + 18.f, y + entryBoxHeight / 2.f + 2.f});
            window.draw(rankText);

            // Username (left-aligned in its column)
            nameText.setString(r.username);
            nameText.setFillColor(sf::Color(60, 30, 0));
            nameText.setStyle(sf::Text::Bold);
            nameText.setOrigin(sf::Vector2f(0.f, 0.f));
            nameText.setPosition(sf::Vector2f(nameColX, y + 10.f));
            window.draw(nameText);

            // Score (right-aligned under "Score" heading)
            scoreText.setString(std::to_string(r.score));
            scoreText.setFillColor(sf::Color(60, 30, 0));
            scoreText.setStyle(sf::Text::Bold);
            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(sf::Vector2f(scoreBounds.size.x, 0)); // right align
            scoreText.setPosition(sf::Vector2f(scoreColX, y + 10.f));
            window.draw(scoreText);

            if (++rank > 10) break; // Show top 10
        }

        // Back instruction
        sf::Text backText(font);
        backText.setString("Press ESC to return");
        backText.setCharacterSize(20);
        backText.setFillColor(sf::Color(120, 80, 40));
        backText.setPosition(sf::Vector2f(windowSize.x / 2.f - 90.f, startY + 10 * (entryBoxHeight + 12.f) + 20.f));
        window.draw(backText);
        break;
    }
}
        
        window.display();
    }

    return 0;
}