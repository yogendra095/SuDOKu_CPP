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
    
    // Load font
    sf::Font font;
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    // Title text
    sf::Text titleText(font);
    titleText.setString("Sudoku Game - Login");
    titleText.setCharacterSize(32);
    titleText.setPosition({250, 50});
    titleText.setFillColor(sf::Color::Black);

    //Title after succesfully login

sf::Text gameTitleText(font);
gameTitleText.setString("SuDoKu");
gameTitleText.setCharacterSize(48);  // Larger size for main title
gameTitleText.setPosition({320, 50});
gameTitleText.setFillColor(sf::Color::Black);
gameTitleText.setStyle(sf::Text::Bold);

    // Menu options
    sf::Text newUserOption(font);
    newUserOption.setString("1. New User");
    newUserOption.setCharacterSize(24);
    newUserOption.setPosition({300, 150});
    newUserOption.setFillColor(sf::Color::Blue);

sf::Text timerText(font);
timerText.setCharacterSize(24);
timerText.setPosition({600, 20}); // top-right corner
timerText.setFillColor(sf::Color::Red);


    sf::Text existingUserOption(font);
    existingUserOption.setString("2. Existing User");
    existingUserOption.setCharacterSize(24);
    existingUserOption.setPosition({300, 200});
    existingUserOption.setFillColor(sf::Color::Blue);

    sf::Text menuPrompt(font);
    menuPrompt.setString("Press 1 or 2 to select:");
    menuPrompt.setCharacterSize(20);
    menuPrompt.setPosition({280, 250});
    menuPrompt.setFillColor(sf::Color::Black);

    // Input prompt
    sf::Text inputPrompt(font);
    inputPrompt.setCharacterSize(24);
    inputPrompt.setPosition({100, 300});
    inputPrompt.setFillColor(sf::Color::Black);

    // Input display
    sf::Text inputDisplay(font);
    inputDisplay.setString("");
    inputDisplay.setCharacterSize(24);
    inputDisplay.setPosition({100, 350});
    inputDisplay.setFillColor(sf::Color::Blue);

    // Status message
    sf::Text statusMessage(font);
    statusMessage.setString("");
    statusMessage.setCharacterSize(20);
    statusMessage.setPosition({100, 450});
    statusMessage.setFillColor(sf::Color::Green);

    // Instructions
    sf::Text instructions(font);
    instructions.setString("Press ESC to go back | Enter to confirm");
    instructions.setCharacterSize(16);
    instructions.setPosition({200, 550});
    instructions.setFillColor(sf::Color::Yellow);

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

                
                // Menu selection (only when in menu state)
                else if (currentState == LoginState::MENU) {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Num1) {
                        currentState = LoginState::NEW_USER_NAME;
                        inputPrompt.setString("Enter your name:");
                        userInput = "";
                        statusMessage.setString("");
                        showInstructions = true;
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Num2) {
                        currentState = LoginState::EXISTING_USER_ID;
                        inputPrompt.setString("Enter your name:");
                        userInput = "";
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
        
        // Only draw Title if Not in logged in state
        if (currentState != LoginState::LOGGED_IN) {
    window.draw(titleText);
}

//IF successfuly login
        if (currentState == LoginState::LOGGED_IN) {
    window.draw(gameTitleText);
        }
        // Clear window
window.clear(sf::Color::White);
        // Draw based on current state
        switch (currentState) {
            case LoginState::MENU:
                window.draw(newUserOption);
                window.draw(existingUserOption);
                window.draw(menuPrompt);
                break;
                
            case LoginState::NEW_USER_NAME:
            case LoginState::EXISTING_USER_ID:
                window.draw(inputPrompt);
                window.draw(inputDisplay);
                window.draw(statusMessage);
                if (showInstructions) {
                    window.draw(instructions);
                }
                break;
                
            case LoginState::LOGGED_IN:
                // Initialize menu once
                if (!menuInitialized) {
                     gameMenu = std::make_unique<Menu>(font);
                    gameMenu->setPosition(300, 200);
                    gameMenu->addButton("Play Sudoku", [&](){
                    LevelSelection levelSelection(font, user.getUnlockedLevel());
                    if(levelSelection.run(window)) {
                        currentLevel = levelSelection.getSelectedLevel();
                            // Create Sudoku game based on difficulty
                         // For simplicity: level 1-2 = Easy, 3-4 = Medium, 5-6 = Hard, 7-8 = Very Hard
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

    case LoginState::PLAYING_SUDOKU:
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
        
        window.display();
    }

    return 0;
}