#include "blocks.hpp"

blocks::blocks(sf::Vector2u window_size) {
    constexpr int rows = 5;
    constexpr int columns = 10;

    game_window_size = window_size;

    initialize_blocks(rows, columns, game_window_size);
}

block::block(sf::Vector2f block_size, sf::Vector2f position, sf::Color color = sf::Color::Blue) {
    shape.setSize(block_size);
    shape.setPosition(position);
    shape.setFillColor(color);
}

hard_block::hard_block(sf::Vector2f block_size, sf::Vector2f position, sf::Color color = sf::Color::Cyan) : block(block_size, position, color) {
}

void blocks::draw(sf::RenderWindow& window) {
    for (const auto& block : block_entities) {
        window.draw(block->get_block_shape());
    }
}

std::vector<std::unique_ptr<block>>& blocks::get_blocks() {
    return block_entities;
}

void blocks::initialize_blocks(int rows, int columns, sf::Vector2u window_size) {
    constexpr float block_gap = 1.f;
    constexpr float margin_x = 10.f;
    constexpr float margin_y = 10.f;
    constexpr float block_area_y_coverage = 0.2f;
    const sf::Vector2f block_area_size(
        window_size.x - margin_x * 2,
        window_size.y * block_area_y_coverage - margin_y);
    const sf::Vector2f block_size(
        block_area_size.x / columns - block_gap,
        block_area_size.y / rows - block_gap);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            sf::Vector2f block_position(
                j * (block_area_size.x / columns) + margin_x,
                i * (block_area_size.y / rows) + margin_y
            );

            if (i == rows - 1) {
                block_entities.push_back(std::make_unique<hard_block>(block_size, block_position));
            } else {
                block_entities.push_back(std::make_unique<block>(block_size, block_position));
            }
            
        }
    }
}

sf::RectangleShape block::get_block_shape() const {
    return shape;
}

void block::update(const sf::CircleShape& ball_shape) {

}

bool block::survive_collision() {
    return false;
}

bool hard_block::survive_collision() {
    if (durability > 0) {
        --durability;
        shape.setFillColor(sf::Color::Blue);
        return true;
    }
    return false;
}