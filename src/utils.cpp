#include "utils.hpp"
#include <math.h>

angle angle::from_radians(float value) {
    return angle(value);
}

angle angle::from_degrees(float value) {
    return angle(value / 180.0f * pi);
}

float angle::as_degrees() const {
    return radians / pi * 180.0f;
}

float angle::as_radians() const {
    return radians;
}

void angle::set_from_degrees(float value) {
    radians = value / 180.f * pi;
}

void angle::set_from_radians(float value) {
    radians = value;
}

bool circle_rectangle_collision(const sf::CircleShape circle, const sf::RectangleShape rectangle) {
    sf::Vector2f circle_center = circle.getPosition() + sf::Vector2f(circle.getRadius(), circle.getRadius());

    sf::Vector2f rectangle_position = rectangle.getPosition();
    sf::Vector2f rectangle_size = rectangle.getSize();

    float closest_x = std::clamp(circle_center.x, rectangle_position.x, rectangle_position.x + rectangle_size.x);
    float closest_y = std::clamp(circle_center.y, rectangle_position.y, rectangle_position.y + rectangle_size.y);

    float dx = circle_center.x - closest_x;
    float dy = circle_center.y - closest_y;

    float distance = hypot(dx, dy);
    float radius = circle.getRadius();

    return distance < radius;
}

void move_shape(sf::Shape& shape, const angle angle, float speed, sf::Time delta_time) {
    constexpr float pi = 3.14159f;
    // Add functions to convert between degrees and radians
    // Add objects radians and degrees to handle angle (konstruktory konwertujące) named constructor idom, strong typing
    float move_x = speed * std::cos(angle.as_radians()) * delta_time.asMilliseconds();
    float move_y = speed * std::sin(angle.as_radians()) * delta_time.asMilliseconds();

    shape.move(sf::Vector2f(move_x, move_y));
}