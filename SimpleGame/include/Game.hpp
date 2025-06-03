#pragma once
#include "Header.hpp"
#include "Player.hpp"
#include "Map.hpp"


class Game 
{
public:
    Game();

public:
    void run();

private:
    void processEvents(float dt);
    void render();
    void mouseControl();
    void keyboardControl(float dt);
    void drawMinimap();

private:
    sf::RenderWindow window;
    Player player;
    Map map;
    sf::Vector2i screen;
};