#pragma once
#include "Header.hpp"
#include "Player.hpp"


class Map 
{
public:
    void init(const sf::Vector2i& screen);
    void castRays(sf::RenderWindow& window, const Player& player);
    char at(int x, int y) const;

private:
    void drawSky(sf::RenderWindow& window, float playerAngle);
    void drawFloor(sf::RenderWindow& window, const Player& player);
    void drawWall(sf::RenderWindow& window, const Player& player);

public:
    std::vector<std::string> worldMap = {
    "################",
    "#..............#",
    "#.......##.....#",
    "#..............#",
    "#..............#",
    "#...##.........#",
    "#...##.........#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#.....####..#..#",
    "#..............#",
    "#..............#",
    "#.......##.....#",
    "#..............#",
    "################"
    };

private:
    int screenX = 0;
    int screenY = 0;

    sf::Texture skyTexture;

    sf::Texture floorTexture;
    sf::Image   floorImage;
    const sf::Uint8* floorData = nullptr;
    sf::Texture floorTextureGPU;
    std::vector<sf::Uint8> pixelBufferFloor;

    sf::Texture wallTexture;
    int wallTexWidth = 0;
    int wallTexHeight = 0;
    sf::VertexArray wallVerts;

    std::vector<float> baseAngle;
    std::vector<float> rayDirX;
    std::vector<float> rayDirY;
};