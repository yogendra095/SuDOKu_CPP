#include <SFML/Graphics.hpp>
#include "User.hpp"
#include <iostream>
#include <string>

enum class LoginState {
    MENU,
    NEW_USER_NAME,
    EXISTING_USER_ID,
    LOGGED_IN
};

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

    // Menu options
    sf::Text newUserOption(font);
    newUserOption.setString("1. New User");
    newUserOption.setCharacterSize(24);
    newUserOption.setPosition({300, 150});
    newUserOption.setFillColor(sf::Color::Blue);

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

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // Escape key handling
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    if (currentState == LoginState::MENU) {
                        window.close();
                    } else if (currentState != LoginState::LOGGED_IN) {
                        // Go back to menu
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
        }

        // Clear and draw
        window.clear(sf::Color::White);
        
        // Always draw title
        window.draw(titleText);
        
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
                // Success screen
                sf::Text successText(font);
                successText.setString("Login Successful!");
                successText.setCharacterSize(28);
                successText.setPosition({280, 200});
                successText.setFillColor(sf::Color::Green);
                window.draw(successText);
                
                sf::Text continueText(font);
                continueText.setString("Game will start here...");
                continueText.setCharacterSize(20);
                continueText.setPosition({290, 250});
                continueText.setFillColor(sf::Color::Black);
                window.draw(continueText);
                
                window.draw(statusMessage);
                break;
        }
        
        window.display();
    }

    return 0;
}