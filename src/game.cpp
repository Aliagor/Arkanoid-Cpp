#include "game.hpp"

arkanoid_game::arkanoid_game(const sf::Vector2u window_size)
    : game_window_size(window_size),
      game_blocks(window_size) {
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
    constexpr float radius = 15.f;

    ball_shape.setRadius(radius);
    ball_shape.setFillColor(sf::Color::Green);
    ball_shape.setPosition(sf::Vector2f(platform_shape.getPosition().x + platform_shape.getSize().x / 2.f - ball_shape.getRadius(),
                                             platform_shape.getPosition().y - ball_shape.getRadius() * 2.f));
}

void arkanoid_game::ball_collision_x() {
    ball_angle = 180.f - ball_angle + std::uniform_int_distribution<int>(-5, 5)(mt);
}

void arkanoid_game::ball_collision_y() {
    ball_angle = -ball_angle + std::uniform_int_distribution<int>(-5, 5)(mt);
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

                ball_angle = -80.f;
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
    float platform_speed = 0.2f;
    float move_distance = platform_speed * delta_time.asMilliseconds();

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

    if (current_game_state == game_state::playing) {
        constexpr float pi = 3.14159f;
        float move_x = ball_speed * std::cos(ball_angle * pi / 180.f) * delta_time.asMilliseconds();
        float move_y = ball_speed * std::sin(ball_angle * pi / 180.f) * delta_time.asMilliseconds();
        ball_shape.move(sf::Vector2f(move_x, move_y));

        // Collisons with sides of screen
        if (ball_shape.getPosition().x < 0 || ball_shape.getPosition().x + ball_shape.getRadius() * 2 > game_window_size.x) {
            if (ball_shape.getPosition().x < 0) {
                ball_shape.setPosition(sf::Vector2f(0.f, ball_shape.getPosition().y));
            }
            else {
                ball_shape.setPosition(sf::Vector2f(game_window_size.x - ball_shape.getRadius() * 2, ball_shape.getPosition().y));
            }
            ball_collision_x();
        }

        // Top of screen collision
        if (ball_shape.getPosition().y < 0) {
            ball_shape.setPosition(sf::Vector2f(ball_shape.getPosition().x, 0.f));
            ball_collision_y();
        }

        // Bottom of screen collision
        if (ball_shape.getPosition().y + ball_shape.getRadius() * 2 > game_window_size.y) {
            current_game_state = game_state::game_over;
        }

        if (circle_rectangle_collision(ball_shape, platform_shape)) {
            ball_shape.setPosition(sf::Vector2f(ball_shape.getPosition().x, platform_shape.getPosition().y - ball_shape.getRadius() * 2.f));
            ball_collision_y();
        }

        std::vector<sf::RectangleShape>& block_shapes = game_blocks.get_block_shapes();

        for (auto it = block_shapes.begin(); it != block_shapes.end(); ++it) {
            if (circle_rectangle_collision(ball_shape, *it)) {
                float top_y_after_collision = ball_shape.getPosition().y > it->getPosition().y ? 
                it->getPosition().y + it->getSize().y + ball_shape.getRadius() * 2 : 
                it->getPosition().y - ball_shape.getRadius() * 2;

                ball_shape.setPosition(sf::Vector2f(ball_shape.getPosition().x, top_y_after_collision));
                ball_collision_y();
                block_shapes.erase(it);
                break;
            }
        }
    }
    ///////////////////// END BALL SECTION
}

void arkanoid_game::draw(sf::RenderWindow& window) {
    window.clear();
    game_blocks.draw(window);
    window.draw(platform_shape);
    window.draw(ball_shape);
    window.display();
}