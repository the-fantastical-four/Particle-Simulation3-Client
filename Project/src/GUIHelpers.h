#pragma once
#include "Particle.h"
#include "Wall.h"
#include <vector>
#include <iostream>

// External declarations for required variables
extern std::vector<Particle> particles;
extern float particle_x, particle_y, particle_angle, particle_speed;
extern float wall_x1, wall_y1, wall_x2, wall_y2;
extern int batch_size_a, batch_size_b, batch_size_c;
extern float batch_start_x_a, batch_start_y_a, batch_end_x_a, batch_end_y_a, batch_speed_a, batch_angle_a;
extern float batch_x_b, batch_y_b, batch_start_angle_b, batch_end_angle_b, batch_speed_b;
extern float batch_x_c, batch_y_c, batch_start_speed_c, batch_end_speed_c, batch_angle_c;

void show_explorer_mode(sf::Sprite& sprite, sf::RenderWindow& window, bool isExplorerMode);
void show_particle_spawner_menu();
void show_batch_spawn_case_1();
void show_batch_spawn_case_2();
void show_batch_spawn_case_3();
void show_reset_button();
