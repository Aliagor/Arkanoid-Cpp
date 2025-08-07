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

enum class upgrade_type {
    extra_balls,
    multiply_balls, // TODO 
    bigger_platform // TODO
};

struct upgrade_entity {
    sf::CircleShape shape;
    upgrade_type type;
};

class ball {
public:
    ball(float angle, const sf::Vector2f position);
    sf::CircleShape& get_shape();
    float get_angle() const;
    float& get_angle();
    float get_speed() const;

protected:
    sf::CircleShape shape;
    float angle;
    float speed = 0.4f;
};

class small_ball: public ball {
public:
    small_ball(float angle, const sf::Vector2f position);
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

    std::mt19937 mt{std::random_device ()()};

    void spawn_upgrade(const sf::Vector2f position);
    void apply_upgrade(const upgrade_entity& upgrade);
    std::vector<upgrade_entity> upgrade_entities;
    
    std::vector<std::unique_ptr<ball>> ball_entities;
    void ball_collision_x(float& ball_angle);
    void ball_collision_y(float& ball_angle);
    
    enum game_state current_game_state = game_state::start;

    void key_pressed(const sf::Event::KeyPressed &keyPressed);
    void key_released(const sf::Event::KeyReleased &keyReleased);
    void init_platform(const sf::Vector2u window_size);
    
    void init_first_ball();
};