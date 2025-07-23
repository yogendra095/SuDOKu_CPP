#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML 3.0 Rectangle Test");

    // Create a rectangle shape (200x100)
    sf::RectangleShape rectangle({200.f, 100.f});
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setPosition({300.f, 250.f}); // Center-ish

    // Main loop
    while (window.isOpen()) {
        // Handle events (SFML 3.0 style)
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Clear the window
        window.clear(sf::Color::White);

        // Draw the rectangle
        window.draw(rectangle);

        // Display the contents
        window.display();
    }

    return 0;
}
