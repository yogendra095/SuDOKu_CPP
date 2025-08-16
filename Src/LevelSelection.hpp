#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

struct LevelButton {
    sf::RectangleShape rect;
    sf::Text text;
    bool locked = true;
    std::function<void()> onClick = [](){};

    // Make button
    LevelButton(const sf::Vector2f& size, const sf::Vector2f& pos, 
                const std::string& label, sf::Font& font, bool isLocked = true)
        : text(font), locked(isLocked)
    {
    // Rectangle
        rect.setSize(size);
        rect.setPosition(pos);
        rect.setFillColor(isLocked ? sf::Color(100,100,100) : sf::Color(70,130,180));
        rect.setOutlineThickness(3.f);
        rect.setOutlineColor(sf::Color::White);

    // Button text
        text.setString(label);
        text.setCharacterSize(24);
        
    // Center text
        auto bounds = text.getLocalBounds();
        text.setOrigin({bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f});
        
    // Put text in middle
        text.setPosition(sf::Vector2f(pos.x + size.x/2.f, pos.y + size.y/2.f));
    }
};

class LevelSelection {
    sf::Font& font;
    std::vector<LevelButton> buttons;
    int unlockedLevel;
    int selectedLevel = 0;

public:
    LevelSelection(sf::Font& font, int unlockedLevel);
    bool run(sf::RenderWindow& window);
    int getSelectedLevel() const { return selectedLevel; }
};