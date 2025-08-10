#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>

class angle {
public:
    static angle from_radians(float value);
    static angle from_degrees(float value);

    float as_radians() const;
    float as_degrees() const;

    void set_from_radians(float value);
    void set_from_degrees(float value);

private:
  angle(float value) : radians(value) {}
  float radians;
  static constexpr float pi = 3.14159f;
};

bool circle_rectangle_collision(const sf::CircleShape circle, const sf::RectangleShape rectangle);

void move_shape(sf::Shape& shape, const angle angle, float speed, sf::Time delta_time);