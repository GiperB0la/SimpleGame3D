#pragma once
#include "Header.hpp"


class Player
{
public:
    float x = 14.7f;
    float y = 5.09f;
    float angle = 0.0f;

    void move(float dx, float dy, const std::vector<std::string>& map) {
        float newX = x + dx;
        float newY = y + dy;
        int ix = (int)newX;
        int iy = (int)newY;
        if (ix >= 0 && ix < MAP_WIDTH && iy >= 0 && iy < MAP_HEIGHT && map[iy][ix] != '#') {
            x = newX;
            y = newY;
        }
    }
};