#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window =  sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Evolution");
    window.setFramerateLimit(60);
	
    sf::Clock clock;

    float dt;
    while (window.isOpen())
    {

    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
    }

	dt = std::min(clock.restart().asSeconds(), 0.05f);



    window.clear();
    window.display();
    }



    return 0;
}