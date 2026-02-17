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
#include "Shroom.h"

static bool isOutsideOfWindow(const sf::RenderWindow& window, const sf::Vector2f& pos)
{
	return pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y;
}


std::list<Banana> bananas;
uint8_t globalBananaId = 0;
int main()
{
	const uint32_t width = sf::VideoMode::getDesktopMode().size.x;
	const uint32_t height = sf::VideoMode::getDesktopMode().size.y;
	sf::RenderWindow window(sf::VideoMode({ width, height }), "Kong");

	const uint32_t framerate = 60;
	window.setFramerateLimit(framerate);

	const auto bananaTexture = std::make_shared<sf::Texture>();
	const auto playerTexture = std::make_shared<sf::Texture>();
	const auto pShroomTexture = std::make_shared<sf::Texture>();
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
	if (!pShroomTexture->loadFromFile("assets/Shroom.png"))
	{
		std::cerr << "Failed to load shroom texture.\n";
		return -1;
	}

	sf::Shader shroomShader;
	if (!shroomShader.loadFromFile("assets/ShroomVertexShader", "assets/ShroomFragmentShader"))
	{
		std::cerr << "Shrooms shader not loaded!\n";
		return -1;
	}
	shroomShader.setUniform("texture", *pShroomTexture);
	shroomShader.setUniform("textureSize", (sf::Vector2f)pShroomTexture->getSize());


	std::vector<Shroom> shrooms;
	//shrooms.emplace_back(sf::Vector2f{(float) width / 2,(float)height / 8 }, shroomTexture, 100, &shroomShader);


	sf::Shader bananaShader;
	if (!bananaShader.loadFromFile("assets/BananaVertexShader", "assets/BananaFragmentShader"))
	{
		std::cerr << "Failed to load banana shader!\n";
		return -1;
	}

	bananaShader.setUniform("texture", *bananaTexture);
	bananaShader.setUniform("maxExplosion", (float) framerate);
	bananaShader.setUniform("textureSize", sf::Vector2f(bananaTexture->getSize().x, bananaTexture->getSize().y));

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
	sf::Text player0Health(font, std::to_string(player0.health));
	player0Health.setPosition({ 10.f, 20.f });
	sf::Text player1Health(font, std::to_string(player1.health));
	player1Health.setPosition({ width - 70.f, 20.f });

	uint8_t playerToThrow = 0;
	int playerWon = -1;
	
	
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
				// Player Won not decided blyat!
				if (event->code == sf::Keyboard::Key::B && playerWon == -1)
				{
					const auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					if (playerToThrow)
					{
						const auto mouseToPlayer = mousePos - player1.getPosition();
						const float distanceModulator = sqrt(mouseToPlayer.length()) / 1000.0f;

						bananas.emplace_back(player1.getPosition(), bananaTexture, globalBananaId++, &bananaShader);
						bananas.back().setVelocity(mouseToPlayer * distanceModulator);
					}
					else
					{
						const auto mouseToPlayer = mousePos - player0.getPosition();
						const float distanceModulator = sqrt(mouseToPlayer.length()) / 1000.0f;

						bananas.emplace_back(player0.getPosition(), bananaTexture, globalBananaId++, &bananaShader);
						bananas.back().setVelocity(mouseToPlayer * distanceModulator);
					}
					playerToThrow = playerToThrow ? 0 : 1;
				}
				else if (event->code == sf::Keyboard::Key::R && playerWon != -1)
				{
					auto it = shrooms.begin();
					for (; it != shrooms.end();)
						it = shrooms.erase(it);
					for (auto bit = bananas.begin(); bit != bananas.end();)
						bit = bananas.erase(bit);
					level.generateSkyline(width, height, 20);
					auto newPositions = level.getPlayerPositions();					
					player0.setPosition(newPositions[0]);					
					player1.setPosition(newPositions[1]);					
					
					player0.health = 100;
					player1.health = 100;

					playerWon = -1;
				}			
			}
		}
		
		window.clear();
		level.draw(window);
		window.draw(player0);
		window.draw(player1);		
		window.draw(player0Health);
		window.draw(player1Health);
		if (playerWon == -1)
		{
			// Deal with banana, baby mama drama!
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
						if (level.isBelowSkyline(pos) ||
							(!playerToThrow && player0.checkIfHitAndDecrementHealth(pos, window)) ||
							(playerToThrow && player1.checkIfHitAndDecrementHealth(pos, window)))
						{
							player0Health.setString(std::to_string(player0.health));
							player1Health.setString(std::to_string(player1.health));
							shouldExplode = true;
							break;
						}
					}
				}
				if (shouldExplode)
				{
					banana.explodingFrame++;
				}

				banana.draw(window, sf::RenderStates::Default);
				if (banana.explodingFrame > framerate || isOutsideOfWindow(window, banana.getPosition()))
				{
					shrooms.emplace_back(banana.getPosition(), pShroomTexture, 1, &shroomShader);
					it = bananas.erase(it);
					if (player0.health <= 0) playerWon = 1;
					else if (player1.health <= 0) playerWon = 0;
				}
				else
					++it;
			}


			// Deal with shrooms, make hallucination rooms
			for (auto& shroom : shrooms)
			{
				shroom.update();
				if (shroom.sporeReleaseCounter > 2 * framerate) {
					bananas.emplace_back(sf::Vector2f{ shroom.mySprite.getPosition().x, shroom.mySprite.getPosition().y - 100.f}, bananaTexture, 1, & bananaShader);
					bananas.back().setVelocity({ playerToThrow ? 10.f : -10.f, 0.f});
					shroom.sporeReleaseCounter = 0;
				}
				shroom.draw(window, sf::RenderStates::Default);
			}
		}
		else
		{
			sf::Text winningText(font, "Winning kurwa player" + std::to_string(playerWon) + "\nPress R to reset kurwa!");
			winningText.setPosition({ 3.f * width / 7, height / 4.f});
			window.draw(winningText);
		}
		
		window.display();
	}
}