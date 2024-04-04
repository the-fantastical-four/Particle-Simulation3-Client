#pragma once

#include <iostream>
#include <vector>
#include <SFML/Network.hpp>
#include <thread>
#include <mutex>
#include "SpriteManager.h"

#define SPRITE 0 
#define PARTICLE 1 

sf::IpAddress serverIp = "25.17.98.165"; 
unsigned short port = 6250; 
sf::TcpSocket serverSocket; 

extern SpriteManager sprite; 

const sf::Vector2f scale = sf::Vector2f(0.5f, 0.5f); 
const std::string spritePath = "include/pikachu.png";

std::vector<SpriteManager*> receiveSpritesAndParticles(std::vector<Particle> &particles) {
    sf::Packet packet;

    std::vector<SpriteManager*> otherSprites;

    if (serverSocket.receive(packet) == sf::Socket::Done) {
        sf::Uint8 messageType; 
        float x, y; 

        particles.clear(); 

        while (packet >> messageType >> x >> y) {
            sf::Vector2f position(x, y);
            if (messageType == SPRITE) {
                SpriteManager* sprite = new SpriteManager(spritePath, scale, position); 
                otherSprites.push_back(sprite);
            }
            else {
                particles.push_back(Particle(position));
            }
        }

    } 

    return otherSprites;
}

void sendSpritePosition(sf::Vector2f newPosition) {
    sf::Packet packet;
    packet << newPosition.x; 
    packet << newPosition.y; 

    if (serverSocket.send(packet) != sf::Socket::Done) {
        std::cerr << "Failed to send packet to server" << std::endl;
    }
}

void connectToServer() {
    // Attempt to connect to the server
    if (serverSocket.connect(serverIp, port) != sf::Socket::Done) {
        std::cerr << "Failed to connect to server" << std::endl;
    }
    else {
        std::cout << "Connected successfully to server" << std::endl;
        serverSocket.setBlocking(true);
    }
}