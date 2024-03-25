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