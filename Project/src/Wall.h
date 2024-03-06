#pragma once
#include <SFML/Graphics.hpp>
extern const int HEIGHT; // Ensure this is defined in one of your .cpp files

struct Wall {
    float x1, y1, x2, y2;
    sf::VertexArray shape;

    Wall(float x1, float y1, float x2, float y2);
};