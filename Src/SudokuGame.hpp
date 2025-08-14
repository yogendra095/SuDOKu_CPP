#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include "User.hpp"

class SudokuGame {
private:
    sf::Font& font;

    std::array<std::array<int,9>,9> puzzle{};
    std::array<std::array<int,9>,9> solution{};
    std::array<std::array<bool,9>,9> fixedCells{};
    std::array<std::array<sf::Color,9>,9> cellColors{};

    int selectedRow = -1;
    int selectedCol = -1;

    int diamonds;
    int timeLimitMinutes;
    
    sf::Clock gameClock;

    void generateBoard(int difficulty);

public:
    bool gameOver=false;
    SudokuGame(sf::Font& font, int difficulty = 1, int diamonds = 3);
  
    void draw(sf::RenderWindow& window);
    void handleMouseClick(sf::Vector2f mousePos);
    void handleKeyboardInput(int number, User& user);
    void handleArrowKeys(sf::Keyboard::Key key);
    bool isCompleted() const;
    int getRemainingTime() const; // in seconds
    int getDiamonds() const { return diamonds; }
};
