#include "SpriteManager.h"
#include <iostream>

SpriteManager::SpriteManager(const std::string& texturePath, const sf::Vector2f& scale, const sf::Vector2f& initialPosition) {
    if (!texture.loadFromFile(texturePath)) {
        std::cout << "Load failed" << std::endl;
        system("pause");
    }

    sprite.setTexture(texture);
    sprite.setScale(scale);
    sprite.setPosition(initialPosition);
}

void SpriteManager::update(sf::RenderWindow& window) {

    const float moveSpeed = 5.0f;
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= moveSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += moveSpeed;

    sf::Vector2f newPosition = sprite.getPosition() + movement;
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    // changed to width and height to avoid particles and sprite going out of bounds in explorer mode
    // when particles are spawned
    float minX = 0, maxX = WIDTH - spriteBounds.width;
    float minY = 0, maxY = HEIGHT - spriteBounds.height;

    newPosition.x = std::min(std::max(newPosition.x, minX), maxX);
    newPosition.y = std::min(std::max(newPosition.y, minY), maxY);

    sprite.setPosition(newPosition);
}

std::future<void> SpriteManager::updateAsync(sf::RenderWindow& window) {
    return std::async(std::launch::async, [this, &window]() {
        update(window);
        });
}

void SpriteManager::draw(sf::RenderWindow& window) {

    // Periphery dimensions in pixels
    const float peripheryWidth = 33 * 10; // 33 columns, each 10 pixels wide
    const float peripheryHeight = 19 * 10; // 19 rows, each 10 pixels tall

    // Center the view on the sprite's current position
    sf::Vector2f center(sprite.getPosition().x + sprite.getGlobalBounds().width / 2, sprite.getPosition().y + sprite.getGlobalBounds().height / 2);
    sf::View view(center, sf::Vector2f(peripheryWidth, peripheryHeight));


    window.setView(view);
    window.draw(sprite);

    //Draw the red border around sprite
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    sf::RectangleShape border(sf::Vector2f(spriteBounds.width, spriteBounds.height)); // 10 is the extra size for the border, adjust as needed

    border.setFillColor(sf::Color::Transparent); // Fill color is transparent, only the border will be visible
    border.setOutlineColor(sf::Color::Red); // Red border color
    border.setOutlineThickness(1); // Thickness of the border, adjust as needed
    border.setPosition(sprite.getPosition().x, sprite.getPosition().y); // Adjust position to align with the sprite, considering the border thickness

    window.draw(border);
}
