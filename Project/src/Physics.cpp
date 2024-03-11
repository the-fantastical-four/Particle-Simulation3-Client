#include "Physics.h"

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

    else if (is_collide) {
        particle.velocity = -particle.velocity;
        particle.shape.move(particle.velocity * delta);
    }
}

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

        // removed wall calculations 

        particle.shape.move(offset);
        handle_collision(particle, sf::Vector2u(static_cast<unsigned int>(WIDTH), static_cast<unsigned int>(HEIGHT)), collide_wall, delta);
    }
}

std::vector<std::future<void>> update_particles(std::vector<Particle>& particles, const std::vector<Wall>& walls) {
    const size_t num_particles = particles.size();
    size_t num_threads = std::thread::hardware_concurrency();

    if (num_threads == 0) {
        num_threads = 1;
    }

    const size_t batch_size = (num_particles + num_threads - 1) / num_threads;

    std::vector<std::future<void>> futures;

    for (size_t start = 0; start < num_particles; start += batch_size) {
        size_t end = std::min(start + batch_size, num_particles);
        auto future = std::async(std::launch::async, update_particle_batch,
            std::ref(particles), std::cref(walls), start, end);
        futures.push_back(std::move(future));
    }

    // return futures
    return futures;
}
