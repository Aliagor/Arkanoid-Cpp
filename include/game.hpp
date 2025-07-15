#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "blocks.hpp"

enum class direction_single_axis {
    left, //change from upper case
    right,
    none
};

enum class game_state {
    start, //change from upper case
    playing
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
    sf::RectangleShape platform_shape;
    enum direction_single_axis platform_direction = direction_single_axis::none;
    
    sf::CircleShape ball_shape;
    
    enum game_state current_game_state = game_state::start;

    void key_pressed(const sf::Event::KeyPressed &keyPressed);
    void key_released(const sf::Event::KeyReleased &keyReleased);
    void init_platform(const sf::Vector2u window_size);
    void init_ball();
};