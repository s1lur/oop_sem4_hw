#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "world.hpp"

int main() 
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
    sf::RenderWindow window(sf::VideoMode(1200, 900), "Player states", sf::Style::Close, settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    double time = 0;
    double dt = 1.0 / 60;

    World world;
    world.addBlock({-500, 770, 20000, 400});
    world.addBlock({-400, 100, 700, 300});
    world.addBlock({600, 500, 300, 120});
    world.addBlock({800, 0, 400, 200});
    world.addBlock({-100, -700, 400, 100});
    world.addBlock({700, -700, 400, 100});
    world.addBlock({1500, -700, 400, 100});
    world.addBlock({1100, -300, 400, 100});

    world.addBlock({1100, 400, 400, 400});

    world.addBlock({1900, -100, 200, 800});

    world.addBlock({3000, 500, 1000, 200});

    world.addEnemy({1700, 700, 50, 50});
    world.addEnemy({1330, 320, 50, 50});
    world.addEnemy({740, 430, 50, 50});
    world.addEnemy({800, 700, 50, 50});
    world.addEnemy({2660, 700, 50, 50});
    world.addEnemy({3200, 430, 50, 50});
    world.addEnemy({3400, 430, 50, 50});
    world.addEnemy({3600, 430, 50, 50});
    world.addEnemy({3800, 430, 50, 50});
    world.addEnemy({1000, -60, 50, 50});
    world.addEnemy({1300, -380, 50, 50});
    world.addEnemy({1700, -780, 50, 50});
    world.addEnemy({900, -780, 50, 50});
    world.addEnemy({100, -780, 50, 50});
    world.addEnemy({-200, -10, 50, 50});
    world.addEnemy({2000, -180, 50, 50});

    while (window.isOpen()) 
    {
        sf::Event event;
        while(window.pollEvent(event)) 
        {
            if(event.type == sf::Event::Closed) 
                window.close();

            world.handleEvents(event);
        }
        window.clear(sf::Color::Black);
        world.update(dt);
        world.draw(window);

        window.display();

        time += dt;
    }

    return 0;
}



