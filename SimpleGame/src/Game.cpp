#include "../include/Game.hpp"


Game::Game() 
    : window(sf::VideoMode::getDesktopMode(), "Raycasting OOP", sf::Style::Fullscreen)
{
    sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();
    screen = sf::Vector2i(fullscreenMode.width, fullscreenMode.height);

    window.setMouseCursorVisible(false);
    sf::Vector2i center = sf::Vector2i(screen.x / 2, screen.y / 2);
    sf::Mouse::setPosition(center, window);

    window.setFramerateLimit(60);

    map.init(screen);
}

void Game::run()
{
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents(dt);
        render();
    }
}

void Game::processEvents(float dt)
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    mouseControl();
    keyboardControl(dt);
}

void Game::render()
{
    window.clear(sf::Color::Blue);
    map.castRays(window, player);
    drawMinimap();
    window.display();
}

void Game::mouseControl()
{
    sf::Vector2i center(screen.x / 2, screen.y / 2);
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2i delta = mousePos - center;

    float sensitivity = 0.0025f;
    player.angle += delta.x * sensitivity;

    if (player.angle < 0) player.angle += 2 * M_PI;
    if (player.angle > 2 * M_PI) player.angle -= 2 * M_PI;

    sf::Mouse::setPosition(center, window);
}

void Game::keyboardControl(float dt)
{
    float forwardX = sin(player.angle);
    float forwardY = cos(player.angle);
    float strafeX = cos(player.angle);
    float strafeY = -sin(player.angle);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        player.move(forwardX * SPEED * dt, forwardY * SPEED * dt, map.worldMap);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        player.move(-forwardX * SPEED * dt, -forwardY * SPEED * dt, map.worldMap);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        player.move(-strafeX * SPEED * dt, -strafeY * SPEED * dt, map.worldMap);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        player.move(strafeX * SPEED * dt, strafeY * SPEED * dt, map.worldMap);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
    }
}

void Game::drawMinimap() {
    const int minimapScale = 4;
    const int minimapOffsetX = 10;
    const int minimapOffsetY = 10;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            sf::RectangleShape tile(sf::Vector2f(minimapScale - 1, minimapScale - 1));
            tile.setPosition(minimapOffsetX + x * minimapScale,
                minimapOffsetY + y * minimapScale);

            char cell = map.worldMap[y][x];
            tile.setFillColor(cell == '#'
                ? sf::Color::White
                : sf::Color(70, 70, 70));
            window.draw(tile);
        }
    }

    sf::CircleShape playerDot(minimapScale / 2.0f);
    playerDot.setFillColor(sf::Color::Red);
    playerDot.setOrigin(minimapScale / 2.0f, minimapScale / 2.0f);
    playerDot.setPosition(
        minimapOffsetX + player.x * minimapScale,
        minimapOffsetY + player.y * minimapScale
    );
    window.draw(playerDot);
}