#include "Physics.h"

void handle_collision(Particle& particle, const sf::Vector2u& window_size, float delta) {
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
}

void update_particle_batch(std::vector<Particle>& particles, size_t start, size_t end) {
    sf::Time elapsed_time = frame_clock.getElapsedTime();
    float delta = elapsed_time.asSeconds();

    for (size_t i = start; i < end; i++) {
        Particle& particle = particles[i];

        sf::Vector2f offset = particle.velocity * delta;

        particle.shape.move(offset);
        handle_collision(particle, sf::Vector2u(static_cast<unsigned int>(WIDTH), static_cast<unsigned int>(HEIGHT)), delta);
    }
}

std::vector<std::future<void>> update_particles(std::vector<Particle>& particles) {
    const size_t num_particles = particles.size();
    size_t num_threads = std::thread::hardware_concurrency();

    if (num_threads <= 0) {
        num_threads = 1;
    }

    const size_t batch_size = (num_particles + num_threads - 1) / num_threads;

    std::vector<std::future<void>> futures;

    for (size_t start = 0; start < num_particles; start += batch_size) {
        size_t end = std::min(start + batch_size, num_particles);
        auto future = std::async(std::launch::async, update_particle_batch,
            std::ref(particles), start, end);
        futures.push_back(std::move(future));
    }

    return futures;
}
