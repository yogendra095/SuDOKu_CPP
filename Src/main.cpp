#include <SFML/Graphics.hpp>
#include "User.hpp"
#include <iostream>
#include <string>
#include "menu.hpp"
#include <memory>
#include "LevelSelection.hpp"
#include "SudokuGame.hpp"

enum class LoginState {
    MENU,
    NEW_USER_NAME,
    EXISTING_USER_ID,
    LOGGED_IN,
    PLAYING_SUDOKU
};
std::unique_ptr<SudokuGame> currentSudoku; 
int currentLevel = 1; 
int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Sudoku Login");

    // Load background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:\\Users\\LOQ\\cpp\\SuDOKu_CPP\\Src\\background.jpg")) {
        return -1;  // Handle error
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // Scale and center background to cover window
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

    // Load font
    sf::Font font;
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    // Title text
    sf::Text titleText(font);
    titleText.setString("Sudoku Game - Login");
    titleText.setCharacterSize(54); // Larger size for emphasis
    titleText.setFillColor(sf::Color(255, 255, 255));
    titleText.setOutlineColor(sf::Color(60, 30, 0));
    titleText.setOutlineThickness(5); // Thicker outline
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);

    // Center the title horizontally (SFML 3 uses position/size)
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x / 2.0f,
        titleBounds.position.y
    ));
    titleText.setPosition(sf::Vector2f(windowSize.x / 2.0f, 40.f));

    // Optional: Add a shadow for the title
    sf::Text titleShadow = titleText;
    titleShadow.setFillColor(sf::Color(0, 0, 0, 120)); // semi-transparent black
    titleShadow.setOutlineThickness(0);
    titleShadow.move(sf::Vector2f(3.f, 3.f)); // offset for shadow

    // Title after successfully login
    sf::Text gameTitleText(font);
    gameTitleText.setString("SuDoKu");
    gameTitleText.setCharacterSize(56);
    gameTitleText.setFillColor(sf::Color(255, 255, 255));
    gameTitleText.setOutlineColor(sf::Color(60, 30, 0));
    gameTitleText.setOutlineThickness(4);
    gameTitleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    gameTitleText.setPosition({windowSize.x / 2.f - 120, 40});

    // Menu options
    sf::Text newUserOption(font);
    newUserOption.setString("New User");
    newUserOption.setCharacterSize(32);
    newUserOption.setFillColor(sf::Color(40, 40, 60)); // Darker text
    newUserOption.setOutlineColor(sf::Color(255, 255, 255, 180)); // Light outline
    newUserOption.setOutlineThickness(2);
    newUserOption.setStyle(sf::Text::Bold);
    newUserOption.setPosition({windowSize.x / 2.f - 100, 150});

    sf::Text existingUserOption(font);
    existingUserOption.setString("Existing User");
    existingUserOption.setCharacterSize(32);
    existingUserOption.setFillColor(sf::Color(40, 40, 60)); // Darker text
    existingUserOption.setOutlineColor(sf::Color(255, 255, 255, 180)); // Light outline
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

    // Input prompt
    sf::Text inputPrompt(font);
    inputPrompt.setCharacterSize(26);
    inputPrompt.setFillColor(sf::Color(255, 255, 255));
    inputPrompt.setOutlineColor(sf::Color(60, 30, 0));
    inputPrompt.setOutlineThickness(2);
    inputPrompt.setPosition({100, 300});

    // Input display
    sf::Text inputDisplay(font);
    inputDisplay.setString("");
    inputDisplay.setCharacterSize(26);
    inputDisplay.setFillColor(sf::Color(255, 255, 180));
    inputDisplay.setOutlineColor(sf::Color(60, 30, 0));
    inputDisplay.setOutlineThickness(2);
    inputDisplay.setPosition({100, 350});

    // Status message
    sf::Text statusMessage(font);
    statusMessage.setString("");
    statusMessage.setCharacterSize(22);
    statusMessage.setFillColor(sf::Color(0, 255, 0));
    statusMessage.setOutlineColor(sf::Color(60, 30, 0));
    statusMessage.setOutlineThickness(2);
    statusMessage.setPosition({100, 450});

    // Instructions
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
            
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // Escape key handling
               if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
    if (currentState == LoginState::MENU) {
        window.close();
    } 
    else if (currentState == LoginState::PLAYING_SUDOKU) {
        // Return to level selection instead of main menu
        LevelSelection levelSelection(font, user.getUnlockedLevel());
        if (levelSelection.run(window)) {
            currentLevel = levelSelection.getSelectedLevel();

            // Create Sudoku game for new level
            int difficulty = 1;
            if(currentLevel >= 3 && currentLevel <= 4) difficulty = 2;
            else if(currentLevel >= 5 && currentLevel <= 6) difficulty = 3;
            else if(currentLevel >= 7) difficulty = 4;

            currentSudoku = std::make_unique<SudokuGame>(font, difficulty, user.getDiamonds());
            currentState = LoginState::PLAYING_SUDOKU;
        } else {
            // If they cancel level selection, go back to menu
            currentState = LoginState::LOGGED_IN;
        }
    }
    else if (currentState != LoginState::LOGGED_IN) {
        // Go back to menu for other states
        currentState = LoginState::MENU;
        userInput = "";
        inputDisplay.setString("");
        statusMessage.setString("");
        showInstructions = true;
    }
}
            }
            
            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                if (currentState == LoginState::NEW_USER_NAME || currentState == LoginState::EXISTING_USER_ID) {
                    char inputChar = static_cast<char>(textEntered->unicode);
                    
                    // Handle backspace
                    if (textEntered->unicode == '\b' && !userInput.empty()) {
                        userInput.pop_back();
                        inputDisplay.setString(userInput);
                    }
                    
                    // Handle Enter key
                    else if (textEntered->unicode == '\r' || textEntered->unicode == '\n') {
                        if (!userInput.empty()) {
                            if (currentState == LoginState::NEW_USER_NAME) {
                                // Create new user
                                user.createNew(userInput);
                                statusMessage.setString("Welcome " + userInput + "! Your ID is: " + user.getId());
                                statusMessage.setFillColor(sf::Color::Green);
                                currentState = LoginState::LOGGED_IN;
                                showInstructions = false;
                            }
                            else if (currentState == LoginState::EXISTING_USER_ID) {
                                // Try to login with name
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
                    
                    // Handle regular text input
                    else if (textEntered->unicode >= 32 && textEntered->unicode < 127) {
                        userInput += inputChar;
                        inputDisplay.setString(userInput);
                    }
                }
            }
            else if (currentState == LoginState::LOGGED_IN && gameMenu) {
             gameMenu->handleEvent(*event, window);
        }
        if (currentState == LoginState::MENU) {
    if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords({mousePressed->position.x, mousePressed->position.y});
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
    }
}
                 if(const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                     if(mousePressed->button == sf::Mouse::Button::Left && currentState == LoginState::PLAYING_SUDOKU){
                        sf::Vector2f mousePos = window.mapPixelToCoords({mousePressed->position.x, mousePressed->position.y});
                        currentSudoku->handleMouseClick(mousePos);
                    }
                 }
                 if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
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
    
              if(num > 0  && currentState == LoginState::PLAYING_SUDOKU) {
         currentSudoku->handleKeyboardInput(num,user);
            }
        }

        }
        

  if (currentState == LoginState::PLAYING_SUDOKU && currentSudoku) {
        if (currentSudoku->isCompleted()) {
            std::cout << "Level Completed!\n";
             int reward = currentLevel; //Diamond
             //For Score
            int baseScore = currentLevel * 100;
            int InitialValue = 50;
            int remainingTime = currentSudoku->getRemainingTime();
            int timeBonus = std::max(0, (remainingTime * baseScore) / InitialValue);
         int finalScore = baseScore + timeBonus;
          if (finalScore < 0) finalScore = 0;
            user.addDiamonds(reward);
             user.addScore(finalScore);
            if (currentLevel == user.getUnlockedLevel() && currentLevel < 8) {
                user.setUnlockedLevel(currentLevel + 1);
            }
            user.save();
            currentSudoku.reset();
            currentState = LoginState::LOGGED_IN;
        } 
        else if (currentSudoku->getRemainingTime() <= 0) {
            std::cout << "Time over!\n";
                user.addDiamonds(currentSudoku->getDiamonds());
                user.save();
            currentSudoku.reset();
            currentState = LoginState::LOGGED_IN;
        }
        else if(currentSudoku->getDiamonds() == 0 && currentSudoku->gameOver){
        std::cout << "Game Over due to zero diamonds!\n";
        currentSudoku.reset();
        currentState = LoginState::LOGGED_IN;
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
        window.draw(inputPrompt);
        window.draw(inputDisplay);
        window.draw(statusMessage);
        if (showInstructions) {
            window.draw(instructions);
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
            gameMenu->addButton("View Ranking", []{});
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
            currentSudoku->draw(window);

            // Display remaining time
            int remainingTime = currentSudoku->getRemainingTime(); // in seconds
            int minutes = remainingTime / 60;
            int seconds = remainingTime % 60;
            timerText.setString("Time: " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds));

            window.draw(timerText); // draw it on top
        }
        break;
    }
}
        
        window.display();
    }

    return 0;
}