#pragma once
#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <SFML/Graphics.hpp>

class SpriteManager {
public:
    SpriteManager(const std::string& texturePath, const sf::Vector2f& scale, const sf::Vector2f& initialPosition);
    void update(sf::RenderWindow& window, bool isExplorerMode);
    void draw(sf::RenderWindow& window, bool isExplorerMode);

private:
    sf::Texture texture;
    sf::Sprite sprite;
};

#endif // SPRITEMANAGER_H
