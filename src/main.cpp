#include "game.hpp"

#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({800u, 600u}), "Arkanoid Cpp");
    arkanoid_game game(window.getSize());
    sf::Clock clock;

    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Time delta_time = clock.getElapsedTime();
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            game.process_event(event.value());
        }

        game.update(clock.restart());
        
        game.draw(window);
    }
}
