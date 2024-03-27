#pragma once

#include <iostream>
#include <vector>
#include <SFML/Network.hpp>
#include <thread>
#include <mutex>
#include "SpriteManager.h"

sf::IpAddress serverIp = "127.0.0.1"; 
unsigned short port = 6250; 
sf::TcpSocket serverSocket; 

extern SpriteManager sprite; 

const sf::Vector2f scale = sf::Vector2f(0.5f, 0.5f); 
const std::string spritePath = "include/pikachu.png";

std::vector<SpriteManager*> receiveSprites() {
    sf::Packet packet;
    std::vector<sf::Vector2f> positions;

    std::vector<SpriteManager*> otherSprites; 
    if (serverSocket.receive(packet) == sf::Socket::Done) {
        float x, y; 

        while (packet >> x >> y) {
            positions.emplace_back(x, y); 
        }

        for (auto position : positions) {
            SpriteManager* spriteManager = new SpriteManager(spritePath, scale, position); 
            otherSprites.push_back(spriteManager); 
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
    else {
        std::cout << "Packet sent to server (" << newPosition.x << ", " << newPosition.y << ")" << std::endl;
    }
}

void connectToServer() {
    // Attempt to connect to the server
    if (serverSocket.connect(serverIp, port) != sf::Socket::Done) {
        std::cerr << "Failed to connect to server" << std::endl;
    }
    else {
        std::cout << "Connected successfully to server" << std::endl;
        serverSocket.setBlocking(false);
    }
}