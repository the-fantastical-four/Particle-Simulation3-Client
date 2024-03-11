#include "GUIHelpers.h"
#include "imgui.h"

void show_explorer_mode(sf::Sprite& sprite, sf::RenderWindow& window, bool isExplorerMode) {
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
}

void show_particle_spawner_menu() {
    ImGui::Text("Spawn Particle");
    ImGui::Columns(2, nullptr, false);
    ImGui::InputFloat("x ", &particle_x);
    ImGui::NextColumn();
    ImGui::InputFloat("y ", &particle_y);
    ImGui::NextColumn();
    ImGui::InputFloat("Angle ", &particle_angle);
    ImGui::NextColumn();
    ImGui::InputFloat("Velocity ", &particle_speed);
    ImGui::Columns(1);
    if (ImGui::Button("Spawn Particle")) {
        particles.push_back(Particle(particle_x, particle_y, particle_speed, particle_angle));
    }

    ImGui::NewLine();
}

void show_batch_spawn_case_1() {
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
    ImGui::InputFloat(" Angle", &batch_angle_a);
    ImGui::NextColumn();
    ImGui::InputFloat(" Velocity", &batch_speed_a);

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
}

void show_batch_spawn_case_2() {
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
}

void show_batch_spawn_case_3() {
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
}

void show_reset_button() {
    if (ImGui::Button("Reset")) {
        particles.clear();
    }
}