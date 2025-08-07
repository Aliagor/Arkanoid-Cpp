#include "game.hpp"

arkanoid_game::arkanoid_game(const sf::Vector2u window_size)
    : game_window_size(window_size),
      game_blocks(window_size) {
    init_platform(window_size);
    init_first_ball();
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

void arkanoid_game::init_first_ball() {
    constexpr float ball_angle = -80.f;
    const sf::Vector2f platform_top_center = sf::Vector2f(platform_shape.getPosition().x + platform_shape.getSize().x / 2.f, platform_shape.getPosition().y);

    ball_entities.push_back(std::make_unique<ball>(ball_angle, platform_top_center));
}

void arkanoid_game::ball_collision_x(float& ball_angle) {
    ball_angle = 180.f - ball_angle + std::uniform_int_distribution<int>(-5, 5)(mt);
}

void arkanoid_game::ball_collision_y(float& ball_angle) {
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

void arkanoid_game::key_pressed(const sf::Event::KeyPressed &keyPressed) {
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
    ///////////////////// PLATFORM SECTION
    float platform_speed = 0.4f;
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
    for (auto& ball : ball_entities) {
        assert(ball);

        sf::CircleShape& ball_shape = ball->get_shape();
        float ball_speed = ball->get_speed();

        if (current_game_state == game_state::start) {
            ball_shape.setPosition(sf::Vector2f(platform_shape.getPosition().x + platform_shape.getSize().x / 2.f - ball_shape.getRadius(),
                                                platform_shape.getPosition().y - ball_shape.getRadius() * 2.f));
        }

        if (current_game_state == game_state::playing) {
            constexpr float pi = 3.14159f;
            // Add functions to convert between degrees and radians
            // Add objects radians and degrees to handle angle (konstruktory konwertujące) named constructor idom, strong typing
            float move_x = ball_speed * std::cos(ball->get_angle() * pi / 180.f) * delta_time.asMilliseconds();
            float move_y = ball_speed * std::sin(ball->get_angle() * pi / 180.f) * delta_time.asMilliseconds();
            ball_shape.move(sf::Vector2f(move_x, move_y));

            // Collisons with sides of screen
            if (ball_shape.getPosition().x < 0 || ball_shape.getPosition().x + ball_shape.getRadius() * 2 > game_window_size.x) {
                if (ball_shape.getPosition().x < 0) {
                    ball_shape.setPosition(sf::Vector2f(0.f, ball_shape.getPosition().y));
                }
                else {
                    ball_shape.setPosition(sf::Vector2f(game_window_size.x - ball_shape.getRadius() * 2, ball_shape.getPosition().y));
                }
                ball_collision_x(ball->get_angle());
            }

            // Top of screen collision
            if (ball_shape.getPosition().y < 0) {
                ball_shape.setPosition(sf::Vector2f(ball_shape.getPosition().x, 0.f));
                ball_collision_y(ball->get_angle());
            }

            // Bottom of screen collision
            if (ball_shape.getPosition().y + ball_shape.getRadius() * 2 > game_window_size.y) {
                continue;
            }

            if (circle_rectangle_collision(ball_shape, platform_shape)) {
                ball_shape.setPosition(sf::Vector2f(ball_shape.getPosition().x, platform_shape.getPosition().y - ball_shape.getRadius() * 2.f));
                ball_collision_y(ball->get_angle());
            }

            auto& blocks = game_blocks.get_blocks();

            for (auto it = blocks.begin(); it != blocks.end(); ++it) {
                it->get()->update(ball_shape);
                // Move everything below to update above later

                sf::RectangleShape block_shape = it->get()->get_block_shape();
                if (circle_rectangle_collision(ball_shape, block_shape)) {
                    float top_y_after_collision = ball_shape.getPosition().y > block_shape.getPosition().y ? 
                    block_shape.getPosition().y + block_shape.getSize().y + ball_shape.getRadius() * 2 : 
                    block_shape.getPosition().y - ball_shape.getRadius() * 2;

                    ball_shape.setPosition(sf::Vector2f(ball_shape.getPosition().x, top_y_after_collision));
                    ball_collision_y(ball->get_angle());

                    if (!it->get()->survive_collision()) {
                        if (std::uniform_int_distribution<int>(1, 100)(mt) <= 20) {
                            sf::Vector2f upgrade_position = block_shape.getPosition() + sf::Vector2f(block_shape.getSize().x / 2.f, 0.f);
                            spawn_upgrade(block_shape.getPosition());
                        }
                        blocks.erase(it);
                        break;
                    }
                }
            }
        }
    }

    ball_entities.erase(
        std::remove_if(ball_entities.begin(), ball_entities.end(),
        [&](std::unique_ptr<ball>& ball) { 
            return ball->get_shape().getPosition().y + ball->get_shape().getRadius() * 2 > game_window_size.y;
        }),
        ball_entities.end()
    );

    // Check if any balls are left, if not game over
    if (ball_entities.empty()) {
        current_game_state = game_state::game_over;
    }

    ///////////////////// END BALL SECTION
    ///////////////////// UPGRADE ENTITY SECTION
    for (auto it = upgrade_entities.begin(); it != upgrade_entities.end();) {
        // Function for moving the shape (input delta_time, speed, direction)
        it->shape.move(sf::Vector2f(0.f, 0.1f * delta_time.asMilliseconds()));

        if (it->shape.getPosition().y > game_window_size.y) {
            it = upgrade_entities.erase(it);
        } else if (circle_rectangle_collision(it->shape, platform_shape)) {
            apply_upgrade(*it);
            it = upgrade_entities.erase(it);
        } else {
            ++it;
        }
    }
    ///////////////////// END UPGRADE ENTITY SECTION
}

void arkanoid_game::spawn_upgrade(const sf::Vector2f position) {
    upgrade_entity upgrade = {sf::CircleShape(10.f)};
    upgrade.shape.setFillColor(sf::Color::Yellow);
    upgrade.shape.setPosition(position);

    // For now only one type of upgrade, roll for it when more are added
    // Losowanie z wagami
    upgrade.type = upgrade_type::extra_balls;

    upgrade_entities.push_back(upgrade);
}

void arkanoid_game::apply_upgrade(const upgrade_entity& upgrade) {
    switch (upgrade.type) {
        case upgrade_type::extra_balls: {
            constexpr float radius = 15.f;
            sf::Vector2f start_position = sf::Vector2f(platform_shape.getPosition().x + platform_shape.getSize().x / 2.f - radius,
                                                    platform_shape.getPosition().y - radius * 2.f);
            for (int i = 0; i < 3; ++i) {
                // Move to separate function
                float ball_angle = -45.f - i * 45.f;
                ball_entities.push_back(std::make_unique<small_ball>(ball_angle, start_position));
            }
        }
            break;
        default:
            break;
    }
}

void arkanoid_game::draw(sf::RenderWindow& window) {
    window.clear();
    game_blocks.draw(window);
    window.draw(platform_shape);
    for (const auto& ball : ball_entities) {
        if (ball) {
            window.draw(ball->get_shape());
        }
    }
    for (const auto& upgrade : upgrade_entities) {
        window.draw(upgrade.shape);
    }

    window.display();
}

ball::ball(float angle, const sf::Vector2f position) : angle(angle) {
    shape.setPosition(position);
    shape.setRadius(10.f);
    shape.setFillColor(sf::Color::Green);
}

small_ball::small_ball(float angle, const sf::Vector2f position) : ball(angle, position) {
    speed = 0.6f;

    shape.setPosition(position);
    shape.setRadius(5.f);
    shape.setFillColor(sf::Color::Red);
}

sf::CircleShape& ball::get_shape() {
    return shape;
}

float ball::get_angle() const {
    return angle;
}

float& ball::get_angle() {
    return angle;
}

float ball::get_speed() const {
    return speed;
}