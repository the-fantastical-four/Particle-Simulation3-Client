#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <future>
#include <thread>
#include "Particle.h"
#include "Wall.h"
#include "Physics.h"
#include "GUIhelpers.h"

using namespace std;

// Window dimensions 
const int WIDTH = 1280;
const int HEIGHT = 720;

// Particle size 
const int SIZE = 4; 

sf::Clock frame_clock;
sf::Clock fpsClock; 

// Variable declarations for input storage
float particle_x = 0.f, particle_y = 0.f, particle_angle = 0.f, particle_speed = 0.f;
float wall_x1 = 0.f, wall_y1 = 0.f, wall_x2 = 0.f, wall_y2 = 0.f;
int batch_size_a = 0, batch_size_b = 0, batch_size_c = 0;
float batch_start_x_a = 0.f, batch_start_y_a = 0.f, batch_end_x_a = 0.f, batch_end_y_a = 0.f, batch_angle_a = 0.f, batch_speed_a = 0.f;
float batch_x_b = 0.f, batch_y_b = 0.f, batch_start_angle_b = 0.f, batch_end_angle_b = 0.f, batch_speed_b = 0.f;
float batch_x_c = 0.f, batch_y_c = 0.f, batch_angle_c = 0.f, batch_start_speed_c = 0.f, batch_end_speed_c = 0.f;

std::vector<Particle> particles;
std::vector<Wall> walls;

void show_frame_rate(float fps) {
    ImGui::Begin("Frame Rate");
    ImGui::Text("Frame Rate: %.2f", fps);
    ImGui::End();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particle Bouncing");
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);
    int frame_count = 0; 
    float fps = 0.f; 

    sf::Clock deltaClock;

    // Load a texture from a file
    sf::Texture texture;
    if (!texture.loadFromFile("include/pikachu.png")) {
        // error handling
        std::cout << "Load failed" << std::endl;
        system("pause");
    }

    // Create a sprite and apply the texture
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(0.5f, 0.5f); 
    sprite.setPosition(0, 0);

    bool isExplorerMode = false;

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

        if (fpsClock.getElapsedTime().asSeconds() >= 0.5f) {
            fps = frame_count / fpsClock.restart().asSeconds(); 
            frame_count = 0; 
        }

        show_frame_rate(fps);

        ImGui::Begin("Menu");

        if (ImGui::Button("Explorer Mode")) {
            isExplorerMode = !isExplorerMode;
            std::cout << "explorer mode: " << isExplorerMode << std::endl;
        }

        ImGui::NewLine();

        if (isExplorerMode) {
            const float moveSpeed = 5.0f; // Adjust speed as needed
            sf::Vector2f movement(0.f, 0.f);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                movement.y -= moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                movement.y += moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                movement.x -= moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                movement.x += moveSpeed;
            }

            // Calculate potential new position
            sf::Vector2f newPosition = sprite.getPosition() + movement;

            // Ensure the sprite stays within the window boundaries
            sf::FloatRect spriteBounds = sprite.getGlobalBounds();
            float minX = 0;
            float maxX = window.getSize().x - spriteBounds.width;
            float minY = 0;
            float maxY = window.getSize().y - spriteBounds.height;

            // Clamp the new position within the window boundaries
            newPosition.x = std::min(std::max(newPosition.x, minX), maxX);
            newPosition.y = std::min(std::max(newPosition.y, minY), maxY);

            sprite.setPosition(newPosition);
        }

        //show_explorer_mode(sprite, window, &isExplorerMode);
        show_particle_spawner_menu();
        show_wall_spawner_menu(); 
        show_batch_spawn_case_1();
        show_batch_spawn_case_2();
        show_batch_spawn_case_3(); 
        show_reset_button();

        ImGui::End();

        // -- END GUI STUFF --

        update_particles(particles, walls); 
        frame_clock.restart();

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

        // Draw the sprite - Ensure this line is added
        if (isExplorerMode) {
            window.draw(sprite);

            //Draw the red border around sprite
            sf::FloatRect spriteBounds = sprite.getGlobalBounds();
            sf::RectangleShape border(sf::Vector2f(spriteBounds.width + 10, spriteBounds.height + 10)); // 10 is the extra size for the border, adjust as needed

            border.setFillColor(sf::Color::Transparent); // Fill color is transparent, only the border will be visible
            border.setOutlineColor(sf::Color::Red); // Red border color
            border.setOutlineThickness(5); // Thickness of the border, adjust as needed
            border.setPosition(sprite.getPosition().x - 5, sprite.getPosition().y - 5); // Adjust position to align with the sprite, considering the border thickness

            window.draw(border);
        }

        // Display the contents of the window
        ImGui::SFML::Render(window);
        window.display();

        frame_count++; 
    }

    ImGui::SFML::Shutdown();
    return 0;
}

