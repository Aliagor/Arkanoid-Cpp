#include "blocks.hpp"

blocks::blocks(sf::Vector2u window_size){
    constexpr int rows = 5;
    constexpr int columns = 10;

    game_window_size = window_size;

    initialize_blocks(rows, columns, game_window_size);
}

void blocks::draw(sf::RenderWindow& window) {
    for (const auto& block : block_shapes) {
        window.draw(block);
    }
}

std::vector<sf::RectangleShape>& blocks::get_block_shapes() {
    return block_shapes;
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
            sf::RectangleShape block_shape(block_size);
            block_shape.setPosition(block_position);
            block_shape.setFillColor(sf::Color::Blue);

            block_shapes.push_back(block_shape);
        }
    }
}