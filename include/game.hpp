#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

enum class direction_single_axis {
    LEFT,
    RIGHT,
    NONE
};

enum class game_state {
    START,
    PLAYING
};

class arkanoid_game {
public:
    arkanoid_game();

    sf::RectangleShape get_platform_shape();
    void process_event(const sf::Event& event);
    void update(sf::Time delta_time);
    void draw(sf::RenderWindow& window);
        
private:
    sf::RectangleShape platform_shape;
    enum direction_single_axis platform_direction = direction_single_axis::NONE;
    
    sf::CircleShape ball_shape;
    
    enum game_state current_game_state = game_state::START;

    void key_pressed(const sf::Event::KeyPressed *keyPressed);
    void key_released(const sf::Event::KeyReleased *keyReleased);
    void init_platform();
    void init_ball();
};