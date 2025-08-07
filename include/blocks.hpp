#pragma once

#include <SFML/Graphics.hpp>
#include "utils.hpp"

class block {
public:
    block(sf::Vector2f block_size, sf::Vector2f position, sf::Color color);
    sf::RectangleShape get_block_shape() const;
    void update(const sf::CircleShape& ball_shape);
    virtual bool survive_collision();
protected:
    sf::RectangleShape shape;
};

class hard_block : public block {
public:
    hard_block(sf::Vector2f block_size, sf::Vector2f position, sf::Color color);
    bool survive_collision() override;
private:
    int durability = 1;
};

class blocks {
public:
    blocks(sf::Vector2u window_size);
    sf::Vector2u game_window_size;
    void draw(sf::RenderWindow& window);
    std::vector<std::unique_ptr<block>>& get_blocks();
    void initialize_blocks(int rows, int columns, sf::Vector2u window_size);

private:
    std::vector<std::unique_ptr<block>> block_entities;
};