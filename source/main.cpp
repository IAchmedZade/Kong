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
#include <memory>

#include "Banana.h"
#include "Player.h"

static bool isOutsideOfWindow(const sf::RenderWindow& window, const sf::Vector2f& pos)
{
	return pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y;
}


std::list<Banana> bananas;
uint8_t globalBananaId = 0;
int main()
{
	const auto bananaTexture = std::make_shared<sf::Texture>();
	const auto playerTexture = std::make_shared<sf::Texture>();
	if (!bananaTexture->loadFromFile("assets/banana.png"))
	{
		std::cerr << "Failed to load banana texture.\n";
		return -1;
	}
	if (!playerTexture->loadFromFile("assets/monkeyy.png"))
	{
		std::cerr << "Failed to load mokey texture.\n";
		return -1;
	}

	

	const uint32_t width = sf::VideoMode::getDesktopMode().size.x;
	const uint32_t height = sf::VideoMode::getDesktopMode().size.y;
	sf::RenderWindow window(sf::VideoMode({ width, height }), "Kong");

	Level level;
	level.generateSkyline(width, height, 20);

	std::vector<sf::Vector2f> playerPositions = level.getPlayerPositions();

	Player player0(playerPositions[0], playerTexture, true);
	Player player1(playerPositions[1], playerTexture, false);
	
	sf::Font font;
	if (!font.openFromFile("./assets/comic.ttf"))
	{
		std::cerr << "Failed to open font file!\n";
		return -1;
	}
	sf::Text player0Health(font, std::to_string(player0.getHealth()));
	player0Health.setPosition({ 10.f, 20.f });
	sf::Text player1Health(font, std::to_string(player1.getHealth()));
	player1Health.setPosition({ width - 70.f, 20.f });

	uint8_t playerToThrow = 0;
	
	const uint32_t framerate = 60;
	window.setFramerateLimit(framerate);
	
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
					if (playerToThrow)
					{
						const auto mouseToPlayer = mousePos - player1.getPosition();
						const float distanceModulator = sqrt(mouseToPlayer.length()) / 1000.0f;

						bananas.emplace_back(player1.getPosition(), bananaTexture, globalBananaId++);
						bananas.back().setVelocity(mouseToPlayer * distanceModulator);
					}
					else
					{
						const auto mouseToPlayer = mousePos - player0.getPosition();
						const float distanceModulator = sqrt(mouseToPlayer.length()) / 1000.0f;

						bananas.emplace_back(player0.getPosition(), bananaTexture, globalBananaId++);
						bananas.back().setVelocity(mouseToPlayer * distanceModulator);
					}
					playerToThrow = playerToThrow ? 0 : 1;
				}
				else if (event->code == sf::Keyboard::Key::R)
				{
					level.generateSkyline(width, height, 20);
					auto newPositions = level.getPlayerPositions();
					player0.setPosition(newPositions[0]);
					player1.setPosition(newPositions[1]);
				}			
			}
		}
		
		window.clear();
		level.draw(window);
		window.draw(player0);
		window.draw(player1);		
		window.draw(player0Health);
		window.draw(player1Health);
		auto it = bananas.begin();
		for (; it != bananas.end(); )
		{
			Banana& banana = *it;
			banana.update();
			bool shouldExplode = banana.explodingFrame > 0;
			if (!shouldExplode)
			{
				for (auto& pos : banana.getShittyBoundingPixels())
				{
					if (level.isBelowSkyline(pos) || (!playerToThrow && player0.checkIfHitAndDecrementHealth(pos)) || (playerToThrow && player1.checkIfHitAndDecrementHealth(pos)))
					{
						player0Health.setString(std::to_string(player0.getHealth()));
						player1Health.setString(std::to_string(player1.getHealth()));
						shouldExplode = true;
						break;
					}
				}
			}
			if (shouldExplode)
			{
				banana.explodingFrame++;
			}			
			
			window.draw(banana);
			if (banana.explodingFrame > framerate || isOutsideOfWindow(window,banana.getPosition()))
				it = bananas.erase(it);
			else
				++it;
		}
		
		

		window.display();
	}
}