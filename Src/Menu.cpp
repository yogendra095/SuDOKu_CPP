#include "menu.hpp"

Menu::Button::Button(const sf::Vector2f &size, const sf::Vector2f &pos,
                     const std::string &label, const sf::Font &font)
    :text(font,label), onClick(nullptr)
{
    box.setSize(size);
    box.setPosition(pos);
    box.setFillColor(sf::Color(70, 70, 70));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color::White);

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    // Center text

    // SFML 3.0.0 - Methods likely expect sf::Vector2f now
    auto textBounds = text.getLocalBounds();

    // Calculate origin as Vector2f
    sf::Vector2f textOrigin(textBounds.position.x + textBounds.size.x / 2.0f,
                            textBounds.position.y + textBounds.size.y / 2.0f);
    text.setOrigin(textOrigin);

    // Calculate position as Vector2f
    sf::Vector2f textPosition(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);
    text.setPosition(textPosition);
}

bool Menu::Button::contains(const sf::Vector2f &point) const
{
    return box.getGlobalBounds().contains(point);
}

void Menu::Button::draw(sf::RenderTarget &target) const
{
    target.draw(box);
    target.draw(text);
}

Menu::Menu(const sf::Font &font) : m_font(font) {}

void Menu::setPosition(float x, float y) {
    m_position = {x, y};
    
    // Update button positions
    for (size_t i = 0; i < m_buttons.size(); i++) {
        sf::Vector2f buttonPos(x, y + i * (m_buttonHeight + m_spacing));
        sf::Vector2f textPos = buttonPos + sf::Vector2f(m_buttonWidth / 2.0f, m_buttonHeight / 2.0f);
        
        m_buttons[i].box.setPosition(buttonPos);
        m_buttons[i].text.setPosition(textPos);
    }
}
void Menu::addButton(const std::string &label, std::function<void()> onClick)
{
    float yPos = m_position.y + m_buttons.size() * (m_buttonHeight + m_spacing);
    m_buttons.emplace_back(
        sf::Vector2f(m_buttonWidth, m_buttonHeight),
        sf::Vector2f(m_position.x, yPos),
        label,
        m_font);
    m_buttons.back().onClick = onClick;
}

void Menu::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    if (const auto *mouseButtonReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseButtonReleased->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                {mouseButtonReleased->position.x, mouseButtonReleased->position.y});

            for (auto &button : m_buttons)
            {
                if (button.contains(mousePos) && button.onClick)
                {
                    button.onClick();
                    break;
                }
            }
        }
    }
}
void Menu::draw(sf::RenderWindow &window)
{
    for (const auto &button : m_buttons)
    {
        button.draw(window);
    }
}