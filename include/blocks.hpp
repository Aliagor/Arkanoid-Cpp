#pragma once

#include <SFML/Graphics.hpp>

class blocks {
public:
    blocks(sf::Vector2u window_size);
    sf::Vector2u game_window_size;
    void draw(sf::RenderWindow& window);
    std::vector<sf::RectangleShape>& get_block_shapes();

private:
    std::vector<sf::RectangleShape> block_shapes;
};