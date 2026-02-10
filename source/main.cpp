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
#include "Player.h"





std::list<Banana> bananas;
int main()
{
	const auto bananaTexture = std::make_shared<sf::Texture>();
	const auto playerTexture = std::make_shared<sf::Texture>();
	if (!bananaTexture->loadFromFile("assets/banana.png"))
	{
		std::cerr << "Failed to load texture.\n";
		return -1;
	}
	if (!playerTexture->loadFromFile("assets/monkeyy.png"))
	{
		std::cerr << "Failed to load texture.\n";
		return -1;
	}

	const uint32_t width = 800;
	const uint32_t height = 600;
	sf::RenderWindow window(sf::VideoMode({ width, height }), "Kong");

	Level level;

	Player player1({ width / 4, height / 2 }, playerTexture);
	Player player2({ 3 * width / 4, height / 2 }, playerTexture);
	std::vector<sf::RectangleShape> skyline = level.generateSkyline(width, height, 20);
	window.setFramerateLimit(60);
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
					const auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

					const auto mouseToPlayer = mousePos - player1.getPosition();
					const float distanceModulator = sqrt(mouseToPlayer.length()) / 1000.0f;

					bananas.emplace_back(player1.getPosition(), bananaTexture);
					bananas.back().setVelocity(mouseToPlayer * distanceModulator);
				}
				else if (event->code == sf::Keyboard::Key::R)
				{
					skyline.clear();
					skyline = level.generateSkyline(width, height, 20);
				}
			}
		}
		
		window.clear();
		for (auto& box : skyline)
			window.draw(box);
		window.draw(player1);
		window.draw(player2);
		for (Banana& banana : bananas)
		{
			banana.update();
			// TODO Shitty banas re-appear blyat! Need to remove from list kurwa!
			bool shouldRender = true;
			for (auto& pos : banana.getShittyBoundingPixels())
				if (level.isBelowSkyline(pos))
				{
					std::cout << "Banana below skyline detected!! " << pos.x << " " << pos.y << '\n';
					shouldRender = false;
					break;
				}
			if (shouldRender)
				window.draw(banana);
		}
		window.display();
	}
}