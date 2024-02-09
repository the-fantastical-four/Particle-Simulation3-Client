#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
using namespace std;

#define WIDTH 1280
#define HEIGHT 720
#define SIZE 4

enum MenuState {
    SPAWN_CASE1_MENU,
    SPAWN_CASE2_MENU,
    SPAWN_CASE3_MENU
};

sf::Clock frameClock;

struct Wall {
    float x1;
    float y1;
    float x2;
    float y2;
    sf::VertexArray shape;

    Wall(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2), shape(sf::Lines, 2) {
        // Set the position of the first point
        shape[0].position = sf::Vector2f(x1, HEIGHT - y1);

        // Set the position of the second point
        shape[1].position = sf::Vector2f(x2, HEIGHT - y2);

        shape[0].color = sf::Color::Yellow;
        shape[1].color = sf::Color::Yellow;
    }
};

struct Particle {
    float x;
    float y; 
    float speed;
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Particle(float x, float y, float speed, float angle) {
        this->x = x; // this is just to track starting position 
        this->y = y; // this is just to track starting posiiton 
        this->speed = speed; 
        shape.setSize(sf::Vector2f(SIZE, SIZE));
        shape.setFillColor(sf::Color::Cyan);
        // adjusting y relative to the size of the particle 
        float adjusted_y = (HEIGHT - SIZE - y <= 0) ? HEIGHT - y : HEIGHT - SIZE - y; 
        shape.setPosition(x, adjusted_y);
        set_angle_velocity(angle);
    }

    // Set angle in degrees
    void set_angle_velocity(float angleDegrees) {
        float angleRadians = angleDegrees * 3.14159 / 180;

        // Set velocity based on speed and angle
        velocity.x = speed * std::cos(angleRadians);
        velocity.y = speed * -std::sin(angleRadians);
    }
};

/*
    This function handles the bouncing of particles on the screen
    edges and walls. 
*/
void handleCollision(Particle& particle, const sf::Vector2u& windowSize, bool is_collide, float delta) {
    sf::FloatRect bounds = particle.shape.getGlobalBounds();

    // Check collisions with window boundaries
    if (bounds.left < 0 || bounds.left + bounds.width > WIDTH) {
        particle.velocity.x = -particle.velocity.x;
        // this part is just to prevent the particle from riding the wall if it spawns on the wall
        if (particle.x == 0) {
            particle.shape.move(particle.velocity * delta);
            particle.x++; 
        } 
    }

    if (bounds.top < 0 || bounds.top + bounds.height > HEIGHT) {
        particle.velocity.y = -particle.velocity.y;
        // this part is just to prevent the particle from riding the wall if it spawns on the wall
        if (particle.y == 0) {
            particle.shape.move(particle.velocity * delta);
            particle.y++; 
        }
    }

    if (is_collide) {
        particle.velocity = -particle.velocity;
        particle.shape.move(particle.velocity * delta); 
    }
}

/*
    This function returns the position of the particle 
    relative to the direction it is moving in. For example,
    if a particle is moving upwards and to the right, its x 
    and y values will be the right bound and top bound of the 
    particle respectively. 
*/
sf::Vector2f get_relative_pos(Particle particle) {
    sf::FloatRect bounds = particle.shape.getGlobalBounds();

    float temp_x;
    float temp_y;

    temp_x = particle.velocity.x > 0 ? bounds.left + bounds.width : bounds.left; 
    temp_y = particle.velocity.y > 0 ? bounds.top + bounds.height : bounds.top; 

    return sf::Vector2f(temp_x, temp_y);
}

/*
    This function determines whether a particle will intersect 
    with a wall given its current position and projected position.
    If the particle will intersect with a wall, it will return a 
    the position that is closest to the wall, otherwise, it will 
    return the original projected position. 
*/
sf::Vector2f get_offset(Particle particle, Wall wall, float delta) {

    sf::Vector2f position = particle.shape.getPosition();  
    sf::Vector2f projection = position + particle.velocity * delta; 
    sf::Vector2f p0 = wall.shape[0].position; 
    sf::Vector2f p1 = wall.shape[1].position; 

    float b = (p1.x - p0.x) * (projection.y - position.y) - (p1.y - p0.y) * (projection.x - position.x); 
    float a = (p1.x - p0.x) * (p0.y - position.y) - (p1.y - p0.y) * (p0.x - position.x); 
    float c = (projection.x - position.x) * (p0.y - position.y) - (projection.y - position.y) * (p0.x - position.x); 

    if (b == 0) {
        return particle.velocity * delta; 
    }

    float alpha = a / b; 
    float beta = c / b; 

    if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1) {
        float x0 = position.x + alpha * (projection.x - position.x); 
        float y0 = position.y + alpha * (projection.y - position.y); 

        return sf::Vector2f(x0, y0) - position; 
    }

    return particle.velocity * delta; 
}

/*
    This function handles updating the position of particles. 
    AKA this is what makes the particles move with collision. 
*/
void updateParticles(std::vector<Particle>& particles, const std::vector<Wall>& walls) {
    sf::Time elapsed_time = frameClock.restart();
    float delta = elapsed_time.asSeconds();

    for (auto& particle : particles) {
        sf::Vector2f offset = particle.velocity * delta;
        bool collide_wall = false; 

        for (auto& wall : walls) {
            sf::Vector2f temp = get_offset(particle, wall, delta); 
            if (temp != offset) {
                offset = temp; 
                collide_wall = true; 
                break; 
            }
        }
        

        particle.shape.move(offset);
        handleCollision(particle, { WIDTH, HEIGHT }, collide_wall, delta);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particle Bouncing");
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    // to store input
    float particle_x = 0.f;
    float particle_y = 0.f;
    float particle_angle = 0.f;
    float particle_speed = 0.f;

    float wall_x1 = 0.f;
    float wall_y1 = 0.f;
    float wall_x2 = 0.f;
    float wall_y2 = 0.f;

    int batch_size = 0; 
    float start_x = 0.f; 
    float start_y = 0.f; 
    float end_x = 0.f; 
    float end_y = 0.f; 
    float start_angle = 0.f; 
    float end_angle = 0.f; 
    float start_speed = 0.f; 
    float end_speed = 0.f; 

    std::vector<Particle> particles;
    std::vector<Wall> walls;

    sf::Clock deltaClock;

    MenuState currentMenu = SPAWN_CASE1_MENU; // Added boolean to switch between menus

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // -- BEGIN GUI STUFF --

        switch (currentMenu) {
        case SPAWN_CASE1_MENU:
            ImGui::Begin("Spawn Case 1 Menu");
            ImGui::Text("Case 1");

            // Input Particle
            ImGui::Columns(2, "Spawn Particle", false);

            ImGui::InputFloat("x", &particle_x);
            ImGui::NextColumn();
            ImGui::InputFloat("y", &particle_y);

            ImGui::NextColumn();

            ImGui::InputFloat("Angle", &particle_angle);
            ImGui::NextColumn();
            ImGui::InputFloat("Velocity", &particle_speed);

            ImGui::Columns(1); // end table

            // Confirm Button
            if (ImGui::Button("Spawn Particle")) {
                Particle particle = Particle(particle_x, particle_y,
                    particle_speed, particle_angle);
                particles.push_back(particle);
            }

            ImGui::NewLine();

            // Input Wall 
            ImGui::Text("Spawn Wall");
            ImGui::Columns(2, "Spawn Wall", false);

            ImGui::InputFloat("x1", &wall_x1);
            ImGui::NextColumn();
            ImGui::InputFloat("y1", &wall_y1);

            ImGui::NextColumn();

            ImGui::InputFloat("x2", &wall_x2);
            ImGui::NextColumn();
            ImGui::InputFloat("y2", &wall_y2);

            ImGui::Columns(1);

            if (ImGui::Button("Spawn Wall")) {
                Wall wall = Wall(wall_x1, wall_y1, wall_x2, wall_y2);
                walls.push_back(wall);
            }

            ImGui::Text("Switch to Case 2 Menu");
            if (ImGui::Button("Switch")) {
                currentMenu = SPAWN_CASE2_MENU;
            }

            ImGui::End();
            break;

        case SPAWN_CASE2_MENU:
            ImGui::Begin("Spawn Case 2 Menu");
            ImGui::Text("Case 2");

            // Input Wall
            ImGui::Columns(2, "Spawn Wall", false);

            ImGui::InputFloat("x1", &wall_x1);
            ImGui::NextColumn();
            ImGui::InputFloat("y1", &wall_y1);

            ImGui::NextColumn();

            ImGui::InputFloat("x2", &wall_x2);
            ImGui::NextColumn();
            ImGui::InputFloat("y2", &wall_y2);

            ImGui::Columns(1);

            // Confirm Button
            if (ImGui::Button("Spawn Wall")) {
                Wall wall = Wall(wall_x1, wall_y1, wall_x2, wall_y2);
                walls.push_back(wall);
            }

            ImGui::NewLine();

            ImGui::Text("Switch to Case 3");
            if (ImGui::Button("Switch")) {
                currentMenu = SPAWN_CASE3_MENU;
            }

            ImGui::End();
            break;

        case SPAWN_CASE3_MENU:
            ImGui::Begin("Spawn Case 3 Menu");
            // Content for the other menu state
            ImGui::Text("Case 3");
            ImGui::Text("You can add your other controls here.");
            ImGui::Text("Switch to Spawn Particle Menu");
            if (ImGui::Button("Switch")) {
                currentMenu = SPAWN_CASE1_MENU;
            }
            ImGui::End();
            break;
        }

        // -- END GUI STUFF --

        // Update particle positions and handle collisions
        updateParticles(particles, walls); // assuming you have a function for this

        // Clear the window
        window.clear();

        // Draw particles
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        }

        // Draw walls
        for (const auto& wall : walls) {
            window.draw(wall.shape);
        }

        // Display the contents of the window
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

