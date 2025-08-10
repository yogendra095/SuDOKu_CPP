#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

class Menu {
public:
    Menu(const sf::Font& font);
    
    void setPosition(float x, float y);
    void addButton(const std::string& label, std::function<void()> onClick);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    
private:
    struct Button {
        sf::RectangleShape box;
        sf::Text text;
        std::function<void()> onClick;
        
        Button(const sf::Vector2f& size, const sf::Vector2f& pos, 
               const std::string& label, const sf::Font& font);
        bool contains(const sf::Vector2f& point) const;
        void draw(sf::RenderTarget& target) const;
    };

    const sf::Font& m_font;
    std::vector<Button> m_buttons;
    sf::Vector2f m_position;
    float m_buttonWidth = 200.f;
    float m_buttonHeight = 50.f;
    float m_spacing = 20.f;
};