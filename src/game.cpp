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
    const sf::Vector2f platform_top_center = sf::Vector2f(platform_shape.getPosition().x + platform_shape.getSize().x / 2.f, platform_shape.getPosition().y);

    ball_entities.push_back(std::make_unique<ball>(angle::from_degrees(-80), platform_top_center));
}

void arkanoid_game::ball_collision_x(angle& ball_angle) {
    float new_angle_degrees = 180.f - ball_angle.as_degrees() + std::uniform_int_distribution<int>(-5, 5)(mt);
    ball_angle.set_from_degrees(new_angle_degrees);
}

void arkanoid_game::ball_collision_y(angle& ball_angle) {
    float new_angle_degrees = -ball_angle.as_degrees() + std::uniform_int_distribution<int>(-5, 5)(mt);
    ball_angle.set_from_degrees(new_angle_degrees);
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

    if (platform_direction == direction_single_axis::left) {
        move_shape(platform_shape, angle::from_degrees(180), platform_speed, delta_time);
    }
    
    if (platform_direction == direction_single_axis::right) {
        move_shape(platform_shape, angle::from_degrees(0), platform_speed, delta_time);
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
            move_shape(ball_shape, ball->get_angle(), ball_speed, delta_time);

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
        constexpr float upgrade_speed = 0.1f;
        move_shape(it->shape, angle::from_degrees(90), upgrade_speed, delta_time);

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
    int weigth_sum = 0;
    upgrade_entity upgrade = {sf::CircleShape(10.f)};
    upgrade.shape.setFillColor(sf::Color::Yellow);
    upgrade.shape.setPosition(position);

    std::map<upgrade_type, int> upgrade_weight_map = {
        {upgrade_type::extra_balls, 1},
        {upgrade_type::multiply_balls, 2}
    };

    for (auto entry : upgrade_weight_map) {
        weigth_sum += entry.second;
    }

    int rolled_number = std::uniform_int_distribution<int>(1, weigth_sum)(mt);

    for (auto entry : upgrade_weight_map) {
        if (rolled_number < entry.second) {
            upgrade.type = entry.first;
            break;
        }
        rolled_number -= entry.second;
    }

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
                ball_entities.push_back(std::make_unique<small_ball>(angle::from_degrees(ball_angle), start_position));
            }
        }
            break;
        case upgrade_type::multiply_balls:
            ball_entities.reserve(ball_entities.size() * 3);
            for (auto& ball : ball_entities) {
                for (int i = 1; i <= 2; ++i) {
                    float new_angle_degrees = ball->get_angle().as_degrees() - 120 * i;
                    ball_entities.push_back(std::make_unique<small_ball>(angle::from_degrees(new_angle_degrees), ball->get_shape().getPosition()));
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

ball::ball(angle angle, const sf::Vector2f position) : angle_value(angle) {
    shape.setPosition(position);
    shape.setRadius(10.f);
    shape.setFillColor(sf::Color::Green);
}

small_ball::small_ball(angle angle, const sf::Vector2f position) : ball(angle, position) {
    speed = 0.6f;

    shape.setPosition(position);
    shape.setRadius(5.f);
    shape.setFillColor(sf::Color::Red);
}

sf::CircleShape& ball::get_shape() {
    return shape;
}

angle ball::get_angle() const {
    return angle_value;
}

angle& ball::get_angle() {
    return angle_value;
}

float ball::get_speed() const {
    return speed;
}