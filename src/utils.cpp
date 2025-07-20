#include "utils.hpp"

bool circle_rectangle_collision(const sf::CircleShape circle, const sf::RectangleShape rectangle) {
    sf::Vector2f circle_center = circle.getPosition() + sf::Vector2f(circle.getRadius(), circle.getRadius());

    sf::Vector2f rectangle_position = rectangle.getPosition();
    sf::Vector2f rectangle_size = rectangle.getSize();

    float closest_x = std::clamp(circle_center.x, rectangle_position.x, rectangle_position.x + rectangle_size.x);
    float closest_y = std::clamp(circle_center.y, rectangle_position.y, rectangle_position.y + rectangle_size.y);

    float dx = circle_center.x - closest_x;
    float dy = circle_center.y - closest_y;

    float distance_squared = dx * dx + dy * dy;
    float radius = circle.getRadius();

    return distance_squared < (radius * radius);
}
