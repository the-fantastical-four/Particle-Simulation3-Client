#pragma once
#include <SFML/Graphics.hpp>

extern const int HEIGHT, SIZE; 

struct Particle {
    float x, y, speed;
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Particle(float x, float y, float speed, float angle);
    Particle(sf::Vector2f position); 
    void set_angle_velocity(float angle_degrees);
};
