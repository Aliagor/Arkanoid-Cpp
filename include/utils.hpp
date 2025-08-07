#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>

bool circle_rectangle_collision(const sf::CircleShape circle, const sf::RectangleShape rectangle);

void move_shape(sf::Shape& shape, const sf::Vector2f direction, float speed, sf::Time delta_time);