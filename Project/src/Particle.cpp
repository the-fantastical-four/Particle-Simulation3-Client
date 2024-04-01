#include "Particle.h"
#include <cmath>

Particle::Particle(float x, float y, float speed, float angle) : x(x), y(y), speed(speed) {
    shape.setSize(sf::Vector2f(SIZE, SIZE));
    shape.setFillColor(sf::Color::White);
    // invert y axis 
    float adjusted_y = (HEIGHT - SIZE - y <= 0) ? HEIGHT - y : HEIGHT - SIZE - y;
    shape.setPosition(x, adjusted_y);
    set_angle_velocity(angle);
}

void Particle::set_angle_velocity(float angle_degrees) {
    float angle_radians = angle_degrees * 3.14159f / 180.0f;
    velocity.x = speed * std::cos(angle_radians);
    velocity.y = speed * -std::sin(angle_radians);
}

Particle::Particle(sf::Vector2f position) {
    shape.setSize(sf::Vector2f(SIZE, SIZE));
    shape.setFillColor(sf::Color::White);
    shape.setPosition(position); 
}
