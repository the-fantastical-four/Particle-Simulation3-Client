#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <future>
#include <thread>

using namespace std;

#define WIDTH 1280
#define HEIGHT 720
#define SIZE 4

sf::Clock frame_clock;
sf::Clock fpsClock; 

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
    void set_angle_velocity(float angle_degrees) {
        float angle_radians = angle_degrees * 3.14159 / 180;

        // Set velocity based on speed and angle
        velocity.x = speed * std::cos(angle_radians);
        velocity.y = speed * -std::sin(angle_radians);
    }
};

/*
    This function handles the bouncing of particles on the screen
    edges and walls. 
*/
void handle_collision(Particle& particle, const sf::Vector2u& window_size, bool is_collide, float delta) {
    sf::FloatRect bounds = particle.shape.getGlobalBounds();

    // Check collisions with window boundaries
    if (bounds.left < 0 || bounds.left + bounds.width > WIDTH) {
        particle.velocity.x = -particle.velocity.x;
        // this part is just to prevent the particle from riding the wall if it spawns on the wall
        particle.shape.move(particle.velocity * delta);
    }

    if (bounds.top < 0 || bounds.top + bounds.height > HEIGHT) {
        particle.velocity.y = -particle.velocity.y;
        // this part is just to prevent the particle from riding the wall if it spawns on the wall
        particle.shape.move(particle.velocity * delta);
    }

    if (is_collide) {
        particle.velocity = -particle.velocity;
        particle.shape.move(particle.velocity * delta); 
    }
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

void update_particle_batch(std::vector<Particle>& particles, const std::vector<Wall>& walls, size_t start, size_t end) {
    sf::Time elapsed_time = frame_clock.getElapsedTime(); // frameClock.reset(); 
    float delta = elapsed_time.asSeconds();

    for (size_t i = start; i < end; i++) {
        Particle& particle = particles[i];

        sf::Vector2f offset = particle.velocity * delta;
        bool collide_wall = false;

        for (const auto& wall : walls) {
            sf::Vector2f temp = get_offset(particle, wall, delta);
            if (temp != offset) {
                offset = temp;
                collide_wall = true;
                break;
            }
        }

        particle.shape.move(offset);
        handle_collision(particle, { WIDTH, HEIGHT }, collide_wall, delta);
    }
}

void update_particles(std::vector<Particle>& particles, const std::vector<Wall>& walls) {
    const size_t num_particles = particles.size();
    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t batch_size = (num_particles + num_threads - 1) / num_threads;

    std::vector<std::future<void>> futures;

    for (size_t start = 0; start < num_particles; start += batch_size) {
        size_t end = std::min(start + batch_size, num_particles);
        auto future = std::async(std::launch::async, update_particle_batch, std::ref(particles), std::cref(walls), start, end);
        futures.push_back(std::move(future));
    }

    // Wait for all asynchronous tasks to complete
    for (auto& future : futures) {
        future.get();
    }
}

void show_frame_rate() {
	ImGui::Begin("Frame Rate");
	ImGui::Text("Frame Rate: %.2f", 1.0f / fpsClock.restart().asSeconds());
	ImGui::End();
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

    int batch_size_a = 1000;
    float batch_start_x_a = 100;//0.f;
    float batch_start_y_a = 100;//0.f;
    float batch_end_x_a = 500;//0.f;
    float batch_end_y_a = 400;//0.f;
    float batch_angle_a = 45;//0.f;
    float batch_speed_a = 100;//0.f;

    int batch_size_b = 1000;
    float batch_x_b = 640;//0.f;
    float batch_y_b = 360;//0.f;
    float batch_start_angle_b = 360.f;//0.f;
    float batch_end_angle_b = 0.f;
    float batch_speed_b = 500;//0.f;

    int batch_size_c = 1000;
    float batch_x_c = 100;//0.f;
    float batch_y_c = 100;//0.f;
    float batch_angle_c = 55;//0.f;
    float batch_start_speed_c = 100.f;
    float batch_end_speed_c = 500.f;


    std::vector<Particle> particles;
    std::vector<Wall> walls;

    sf::Clock deltaClock;

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

        show_frame_rate();
        ImGui::Begin("Menu");
        ImGui::Text("Spawn Particle");

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

        ImGui::NewLine();

        //BATCH SPAWN CASE 1
        // Input Particle
        ImGui::Text("Case 1 Batch Spawn Particle");
        ImGui::Columns(2, "Spawn Batch Particle", false);
        ImGui::InputInt("Batch Size ", &batch_size_a);
        ImGui::NextColumn(); 
        ImGui::NextColumn();
        ImGui::InputFloat("Start x ", &batch_start_x_a);
        ImGui::NextColumn();
        ImGui::InputFloat("Start y ", &batch_start_y_a);
        ImGui::NextColumn();
        ImGui::InputFloat("End x ", &batch_end_x_a);
        ImGui::NextColumn();
        ImGui::InputFloat("End y ", &batch_end_y_a);
        ImGui::NextColumn();
        ImGui::InputFloat("Angle ", &batch_angle_a);
        ImGui::NextColumn();
        ImGui::InputFloat("Velocity ", &batch_speed_a);

        ImGui::Columns(1); // end table

        // Confirm Button for batch spawning
        if (ImGui::Button("Spawn Case 1")) {
            // Calculate the distance between start and end points
            float distance_x = batch_end_x_a - batch_start_x_a;
            float distance_y = batch_end_y_a - batch_start_y_a;
            float total_distance = sqrt(distance_x * distance_x + distance_y * distance_y);


            // Calculate the step size for evenly distributing particles
            float step_size = 1;
            if (batch_size_a > 1) {
                step_size = total_distance / (batch_size_a - 1);
            }

            // Calculate the angle of the line
            float line_angle = atan2(distance_y, distance_x);

            // Spawn particles along the line
            for (int i = 0; i < batch_size_a; ++i) {
                float spawn_x = batch_start_x_a + i * step_size * cos(line_angle);
                float spawn_y = batch_start_y_a + i * step_size * sin(line_angle);

                Particle particle = Particle(spawn_x, spawn_y, batch_speed_a, batch_angle_a);
                particles.push_back(particle);
            }
        }

        //BATCH SPAWN CASE 2
        // Input Particle
        ImGui::NewLine();
        ImGui::Text("Case 2: Batch Spawn Particle");
        ImGui::Columns(2, "Case 2: Spawn Batch Particle", false);

        ImGui::InputInt("Batch Size  ", &batch_size_b);
        ImGui::NextColumn();
        ImGui::NextColumn(); 
        ImGui::InputFloat("x  ", &batch_x_b);
        ImGui::NextColumn();
        ImGui::InputFloat("y  ", &batch_y_b);
        ImGui::NextColumn();

        ImGui::InputFloat("Start Angle  ", &batch_start_angle_b);
        ImGui::NextColumn();
        ImGui::InputFloat("End Angle  ", &batch_end_angle_b);
        ImGui::NextColumn();
        ImGui::InputFloat("Velocity  ", &batch_speed_b);

        ImGui::Columns(1); // end table

        // Confirm Button for batch spawning
        if (ImGui::Button("Spawn Case 2")) {
            float angle_increment = batch_end_angle_b - batch_start_angle_b / (batch_size_b - 1);
            float current_angle = batch_start_angle_b;

            for (int i = 0; i < batch_size_b; ++i) {
                float spawn_x = batch_x_b;
                float spawn_y = batch_y_b;

                Particle particle = Particle(spawn_x, spawn_y, batch_speed_b, current_angle);
                particles.push_back(particle);

                current_angle += angle_increment;
            }
        }

        //Case 3
        ImGui::NewLine();
        ImGui::Text("Case 3: Batch Spawn Particle");
        ImGui::Columns(2, "Case 3: Spawn Batch Particle", false);

        ImGui::InputInt("Batch Size", &batch_size_c);
        ImGui::NextColumn();
        ImGui::NextColumn(); 
        ImGui::InputFloat("x", &batch_x_c);
        ImGui::NextColumn();
        ImGui::InputFloat("y", &batch_y_c);
        ImGui::NextColumn();

        ImGui::InputFloat("Start Velocity", &batch_start_speed_c);
        ImGui::NextColumn();
        ImGui::InputFloat("End Velocity", &batch_end_speed_c);
        ImGui::NextColumn();
        ImGui::InputFloat("Angle", &batch_angle_c);

        ImGui::Columns(1); // end table

        // Confirm Button for batch spawning
        if (ImGui::Button("Spawn Case 3")) {
            // Calculate the velocity increment for each particle
            float velocity_increment = 1;
            if (batch_size_c > 1) {
                velocity_increment = (batch_end_speed_c - batch_start_speed_c) / (batch_size_c - 1);
            }
            for (int i = 0; i < batch_size_c; ++i) {
                // Interpolate velocity for the current particle
                float current_velocity = batch_start_speed_c + i * velocity_increment;

                Particle particle = Particle(batch_x_c, batch_y_c, current_velocity, batch_angle_c);
                particles.push_back(particle);
            }
        }

        if (ImGui::Button("Reset")) {
            particles.clear();
            walls.clear(); 
        }

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

        // Display the contents of the window
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

