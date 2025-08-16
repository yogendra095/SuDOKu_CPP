#include "LevelSelection.hpp"

LevelSelection::LevelSelection(sf::Font& font, int unlockedLevel)
    : font(font), unlockedLevel(unlockedLevel)
{
    float startX = 100.f;
    float startY = 100.f;
    float buttonWidth = 150.f;
    float buttonHeight = 150.f;
    float spacing = 20.f;
    int buttonsPerRow = 4; // wrap after 4 buttons

    for (int i = 1; i <= 8; ++i) {
        int row = (i-1) / buttonsPerRow;
        int col = (i-1) % buttonsPerRow;

        sf::Vector2f pos(startX + col * (buttonWidth + spacing),
                         startY + row * (buttonHeight + spacing));

        bool locked = (i > unlockedLevel);
        LevelButton btn({buttonWidth, buttonHeight}, pos,
                        "Level " + std::to_string(i),
                        font, locked);

        if (!locked) {
            btn.onClick = [this, i]() { selectedLevel = i; };
        }

        buttons.push_back(btn);
    }
}



bool LevelSelection::run(sf::RenderWindow& window) {
    selectedLevel = 0;

    while (window.isOpen() && selectedLevel == 0) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                return false;
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    return false;
                }
            } else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords({mouseButtonPressed->position.x, mouseButtonPressed->position.y});
                    for (auto& btn : buttons) {
                        if (!btn.locked && btn.rect.getGlobalBounds().contains(mousePos)) {
                            btn.onClick();
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::White);
        for (auto& btn : buttons) {
            window.draw(btn.rect);
            window.draw(btn.text);
        }
        window.display();
    }

    return selectedLevel != 0;
}
