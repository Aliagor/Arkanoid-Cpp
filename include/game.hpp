#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <cmath>
#include <random>

#include "blocks.hpp"
#include "utils.hpp"

enum class direction_single_axis {
    left,
    right,
    none
};

enum class game_state {
    start,
    playing,
    game_over
};

class arkanoid_game {
public:
    arkanoid_game(const sf::Vector2u window_size);

    sf::RectangleShape get_platform_shape() const;
    void process_event(const sf::Event& event);
    void update(sf::Time delta_time);
    void draw(sf::RenderWindow& window);
        
private:
    sf::Vector2u game_window_size;
    blocks game_blocks;
    sf::RectangleShape platform_shape;
    enum direction_single_axis platform_direction = direction_single_axis::none;

    std::random_device rd;
    std::mt19937 mt{rd()};
    
    sf::CircleShape ball_shape;
    float ball_angle = 0;
    float ball_speed = 0.5f;
    void ball_collision_x();
    void ball_collision_y();
    
    enum game_state current_game_state = game_state::start;

    void key_pressed(const sf::Event::KeyPressed &keyPressed);
    void key_released(const sf::Event::KeyReleased &keyReleased);
    void init_platform(const sf::Vector2u window_size);
    void init_ball();
};