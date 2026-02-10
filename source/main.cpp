#include <sfml/network.hpp>
#include <sfml/window.hpp>
#include <sfml/graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <thread>
#include "Level.h"
#include <chrono>

#include "Banana.h"





std::list<Banana> bananas;
int main()
{
	const auto bananaTexture = std::make_shared<sf::Texture>();
	if (!bananaTexture->loadFromFile("assets/banana.png"))
	{
		std::cerr << "Failed to load texture.\n";
		return -1;
	}

	const uint32_t width = 800;
	const uint32_t height = 600;
	sf::RenderWindow window(sf::VideoMode({ width, height }), "Kong");

	Level level;


	sf::RectangleShape rect({ 100,100 });
	rect.setFillColor(sf::Color::Green);
	rect.setOrigin({ 50, 50 });
	rect.setPosition({ width / 4, height / 2 });


	sf::RectangleShape rect2({ 100,100 });
	rect2.setFillColor(sf::Color::Red);
	rect2.setOrigin({ 50, 50 });
	rect2.setPosition({ 3 * width / 4, height / 2 });
	std::vector<sf::RectangleShape> skyline = level.generateSkyline(width, height, 20);
	
	while (window.isOpen())
	{
		const std::optional<sf::Event> optionalevent = window.pollEvent();
		if (optionalevent.has_value())
		{
			sf::Event e = optionalevent.value();
			if (e.is<sf::Event::Closed>() || (e.is<sf::Event::KeyPressed>() && 
				e.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
				return 0;			
			if (e.is<sf::Event::MouseWheelScrolled>())
			{
				float delta = e.getIf<sf::Event::MouseWheelScrolled>()->delta;
				
			}
			if (auto event = e.getIf<sf::Event::KeyReleased>())
			{
				if (event->code == sf::Keyboard::Key::B)
				{
					bananas.emplace_back(sf::Vector2f(50.0f, 140.0f), bananaTexture);
					bananas.back().setVelocity({ 3.0f, -2.0f });
				}
				else if (event->code == sf::Keyboard::Key::R)
				{
					skyline.clear();
					skyline = level.generateSkyline(width, height, 20);
				}
			}
		}
		window.clear();
		window.draw(rect);
		window.draw(rect2);
		for (auto& box : skyline)
			window.draw(box);
		for (Banana& banana : bananas)
		{
			banana.update();
			window.draw(banana);
		}
		window.display();
	}
}