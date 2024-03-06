#include "Wall.h"

Wall::Wall(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2), shape(sf::Lines, 2) {
    // Set the position of the first point
    shape[0].position = sf::Vector2f(x1, HEIGHT - y1);

    // Set the position of the second point
    shape[1].position = sf::Vector2f(x2, HEIGHT - y2);

    shape[0].color = sf::Color::Yellow;
    shape[1].color = sf::Color::Yellow;
}
