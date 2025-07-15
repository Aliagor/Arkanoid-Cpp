#include "game.hpp"

arkanoid_game::arkanoid_game(const sf::Vector2u window_size) {
    game_window_size = window_size;
    init_platform(window_size);
    init_ball();
}

void arkanoid_game::init_platform(const sf::Vector2u window_size) {
    constexpr float platform_size_x = 100.f;
    constexpr float platform_size_y = 20.f;
    float start_position_x = (window_size.x - platform_size_x) / 2;
    float start_position_y = window_size.y - platform_size_y * 2;

    platform_shape.setSize(sf::Vector2f(platform_size_x, platform_size_y));
    platform_shape.setFillColor(sf::Color::White);
    platform_shape.setPosition(sf::Vector2f(start_position_x, start_position_y));
}

void arkanoid_game::init_ball() {
    constexpr float radius = 10.f;

    ball_shape.setRadius(radius);
    ball_shape.setFillColor(sf::Color::Green);
    ball_shape.setPosition(sf::Vector2f(platform_shape.getPosition().x + platform_shape.getSize().x / 2.f - ball_shape.getRadius(),
                                             platform_shape.getPosition().y - ball_shape.getRadius() * 2.f));
}

sf::RectangleShape arkanoid_game::get_platform_shape() const {
    return platform_shape;
}

void arkanoid_game::process_event(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        key_pressed(*keyPressed);
    }

    if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        key_released(*keyReleased);
    }
}

void arkanoid_game::key_pressed(const sf::Event::KeyPressed &keyPressed) { // Change to reference
    switch (keyPressed.code) {
        case sf::Keyboard::Key::Left:
            platform_direction = direction_single_axis::left;
            break;
        case sf::Keyboard::Key::Right:
            platform_direction = direction_single_axis::right;
            break;
        case sf::Keyboard::Key::Space:
            if (current_game_state == game_state::start) {
                current_game_state = game_state::playing;
            }
            break;
        default:
            break;
    }
}

void arkanoid_game::key_released(const sf::Event::KeyReleased &keyReleased) {
    switch (keyReleased.code) {
        case sf::Keyboard::Key::Left:
            if (platform_direction == direction_single_axis::left) {
                platform_direction = direction_single_axis::none;
            }
            break;
        case sf::Keyboard::Key::Right:
            if (platform_direction == direction_single_axis::right) {
                platform_direction = direction_single_axis::none;
            };
            break;
        default:
            break;
    }
}

void arkanoid_game::update(sf::Time delta_time) {
    ///////////////////// PLATFORM SECTION MOVE TO SEPARATE FUNCTION LATER
    float move_distance = 0.2f * delta_time.asMilliseconds();

    if (platform_direction == direction_single_axis::left) {
        platform_shape.move(sf::Vector2f(-move_distance, 0.f));
    }
    
    if (platform_direction == direction_single_axis::right) {
        platform_shape.move(sf::Vector2f(move_distance, 0.f));
    }

    // Collisons with screen
    if (platform_shape.getPosition().x < 0.f) {
        platform_shape.setPosition(sf::Vector2f(0.f, platform_shape.getPosition().y));
    } else if (platform_shape.getPosition().x + platform_shape.getSize().x > game_window_size.x) {
        platform_shape.setPosition(sf::Vector2f(game_window_size.x - platform_shape.getSize().x, platform_shape.getPosition().y));
    }
    ///////////////////// END PLATFORM SECTION
    ///////////////////// BALL SECTION
    if (current_game_state == game_state::start) {
        ball_shape.setPosition(sf::Vector2f(platform_shape.getPosition().x + platform_shape.getSize().x / 2.f - ball_shape.getRadius(),
                                             platform_shape.getPosition().y - ball_shape.getRadius() * 2.f));
    }
    ///////////////////// END BALL SECTION
}

void arkanoid_game::draw(sf::RenderWindow& window) {
    window.clear();
    window.draw(platform_shape);
    window.draw(ball_shape);
    window.display();
}