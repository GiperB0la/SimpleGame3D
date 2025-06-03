#include "../include/Map.hpp"


void Map::init(const sf::Vector2i& screen)
{
    screenX = screen.x;
    screenY = screen.y;

    if (!skyTexture.loadFromFile("Graphics\\sky.png")) {
        std::cerr << "Failed to load sky texture\n";
        std::exit(1);
    }
    skyTexture.setRepeated(true);

    if (!floorTexture.loadFromFile("Graphics\\floor.png")) {
        std::cerr << "[Error] Failed to load floor.png\n";
        std::exit(1);
    }
    floorTexture.setRepeated(true);
    floorImage = floorTexture.copyToImage();
    floorData = floorImage.getPixelsPtr();

    if (!floorTextureGPU.create(screenX, screenY)) {
        std::cerr << "[Error] Failed to load floorTextureGPU\n";
        std::exit(1);
    }

    if (!wallTexture.loadFromFile("Graphics\\wall.png")) {
        std::cerr << "[Error] Failed to load wall.png\n";
        std::exit(1);
    }
    wallTexture.setRepeated(true);
    wallTexWidth = wallTexture.getSize().x;
    wallTexHeight = wallTexture.getSize().y;

    pixelBufferFloor.resize(screenX * screenY * 4);

    wallVerts.setPrimitiveType(sf::Quads);
    wallVerts.resize(screenX * 4);

    baseAngle.resize(screenX);
    rayDirX.resize(screenX);
    rayDirY.resize(screenX);

    for (int x = 0; x < screenX; ++x) {
        baseAngle[x] = -FOV / 2.0f + ((float)x / (float)screenX) * FOV;
    }
}

char Map::at(int x, int y) const 
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return '#';
    return worldMap[y][x];
}

void Map::castRays(sf::RenderWindow& window, const Player& player)
{
    drawFloor(window, player);
    drawSky(window, player.angle);
    drawWall(window, player);
}

void Map::drawSky(sf::RenderWindow& window, float playerAngle)
{
    float angleNorm = std::fmod(playerAngle / (2.0f * M_PI), 1.0f);
    if (angleNorm < 0) angleNorm += 1.0f;

    int texWidth = skyTexture.getSize().x;
    int texHeight = skyTexture.getSize().y;

    int offsetX = static_cast<int>(angleNorm * texWidth);
    sf::IntRect textureRect(offsetX, 0, screenX, texHeight);

    sf::Sprite skySprite(skyTexture, textureRect);
    skySprite.setScale(
        float(screenX) / float(textureRect.width),
        float(screenY / 2) / float(texHeight)
    );
    skySprite.setPosition(0, 0);

    window.draw(skySprite);
}

void Map::drawFloor(sf::RenderWindow& window, const Player& player)
{
    int mid = screenY / 2;

    float rayDirX0 = std::sin(player.angle - FOV / 2.0f);
    float rayDirY0 = std::cos(player.angle - FOV / 2.0f);
    float rayDirX1 = std::sin(player.angle + FOV / 2.0f);
    float rayDirY1 = std::cos(player.angle + FOV / 2.0f);

    int floorW = floorImage.getSize().x;
    int floorH = floorImage.getSize().y;

    for (int y = mid + 1; y < screenY; ++y) {
        float rowDist = (float)screenY / (2.0f * y - screenY);

        float floorXStep = (rayDirX1 - rayDirX0) * rowDist / (float)screenX;
        float floorYStep = (rayDirY1 - rayDirY0) * rowDist / (float)screenX;

        float curFloorX = player.x + rayDirX0 * rowDist;
        float curFloorY = player.y + rayDirY0 * rowDist;

        for (int x = 0; x < screenX; ++x) {
            int tx = (int)(curFloorX * floorW) % floorW;
            int ty = (int)(curFloorY * floorH) % floorH;
            if (tx < 0) tx += floorW;
            if (ty < 0) ty += floorH;

            int srcIdx = (ty * floorW + tx) * 4;
            int dstIdx = 4 * (y * screenX + x);

            pixelBufferFloor[dstIdx + 0] = floorData[srcIdx + 0];
            pixelBufferFloor[dstIdx + 1] = floorData[srcIdx + 1];
            pixelBufferFloor[dstIdx + 2] = floorData[srcIdx + 2];
            pixelBufferFloor[dstIdx + 3] = 255;

            curFloorX += floorXStep;
            curFloorY += floorYStep;
        }
    }

    floorTextureGPU.update(pixelBufferFloor.data());
    sf::Sprite floorSprite(floorTextureGPU);
    window.draw(floorSprite);
}

void Map::drawWall(sf::RenderWindow& window, const Player& player)
{
    int mid = screenY / 2;

    for (int x = 0; x < screenX; ++x) {
        float actualAngle = baseAngle[x] + player.angle;
        rayDirX[x] = std::sin(actualAngle);
        rayDirY[x] = std::cos(actualAngle);
    }

    for (int x = 0; x < screenX; ++x) {
        float eyeX = rayDirX[x];
        float eyeY = rayDirY[x];

        float distanceToWall = 0.0f;
        bool hitWall = false;

        int testX = (int)player.x;
        int testY = (int)player.y;
        int lastX = testX;
        int lastY = testY;

        while (!hitWall && distanceToWall < DEPTH) {
            lastX = testX;
            lastY = testY;
            distanceToWall += 0.05f;

            float checkX = player.x + eyeX * distanceToWall;
            float checkY = player.y + eyeY * distanceToWall;

            testX = (int)checkX;
            testY = (int)checkY;

            if (testX < 0 || testX >= MAP_WIDTH || testY < 0 || testY >= MAP_HEIGHT) {
                hitWall = true;
                distanceToWall = DEPTH;
            }
            else if (worldMap[testY][testX] == '#') {
                hitWall = true;
            }
        }

        float ceiling = ((float)screenY / 2.0f) - ((float)screenY / distanceToWall);
        float floor_ = (float)screenY - ceiling;

        float hitX = player.x + eyeX * distanceToWall;
        float hitY = player.y + eyeY * distanceToWall;

        float sampleX;
        if (testX != lastX)
            sampleX = hitY - std::floor(hitY);
        else
            sampleX = hitX - std::floor(hitX);
        sampleX = std::clamp(sampleX, 0.0f, 0.999f);

        int texX = (int)(sampleX * (float)wallTexWidth);

        int idx = x * 4;
        float xf = (float)x;
        float xf1 = (float)(x + 1);

        float u0 = texX / (float)wallTexWidth;
        float u1 = (texX + 1) / (float)wallTexWidth;
        float v0 = 0.0f;
        float v1 = (float)wallTexHeight;

        wallVerts[idx + 0].position = { xf,    ceiling };
        wallVerts[idx + 0].texCoords = { u0 * wallTexWidth,  v0 };

        wallVerts[idx + 1].position = { xf1,   ceiling };
        wallVerts[idx + 1].texCoords = { u1 * wallTexWidth,  v0 };

        wallVerts[idx + 2].position = { xf1,   floor_ };
        wallVerts[idx + 2].texCoords = { u1 * wallTexWidth,  v1 };

        wallVerts[idx + 3].position = { xf,    floor_ };
        wallVerts[idx + 3].texCoords = { u0 * wallTexWidth,  v1 };
    }

    window.draw(wallVerts, &wallTexture);
}