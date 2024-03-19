#pragma once
#include "Particle.h"
#include "Wall.h"
#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <SFML/Graphics.hpp>

extern const int HEIGHT; 
extern const int WIDTH;
extern sf::Clock frame_clock; 

/*
    This function handles the bouncing of particles on the screen
    edges and walls.
*/
void handle_collision(Particle& particle, const sf::Vector2u& window_size, float delta);


void update_particle_batch(std::vector<Particle>& particles, size_t start, size_t end);


std::vector<std::future<void>> update_particles(std::vector<Particle>& particles, bool isExplorerMode);
