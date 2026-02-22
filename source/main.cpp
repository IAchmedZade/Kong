#include <sfml/network.hpp>
#include <sfml/window.hpp>
#include <sfml/graphics.hpp>
#include <sfml/Graphics/Sprite.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <thread>
#include "Level.h"
#include <chrono>
#include <memory>
#include <stdlib.h>

#include "Banana.h"
#include "Player.h"
#include "Shroom.h"

static bool isOutsideOfWindow(const sf::RenderWindow& window, const sf::Vector2f& pos)
{
	return pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y;
}


struct Spore
{
	sf::Sprite sporeSprite;
	sf::Texture* sporeTexture;
	float blinking = 0.f;
	bool visible = false;

	sf::Shader* sporeShader;
	sf::Vector2f velocity;
	const float xScale = 0.15f;
	const float yScale = 0.2f;

	Spore(sf::Texture* texture, sf::Shader* sporeShader, sf::Vector2f position, sf::Vector2f velocity)
		: sporeSprite(*texture),
		sporeTexture(texture),
		sporeShader(sporeShader),
		velocity(velocity)

	{
		sporeSprite.setTexture(*texture);
		sporeSprite.setPosition(position);
		sporeSprite.setScale({ xScale, yScale });
		sporeSprite.setOrigin({ 0.5f * sporeSprite.getTextureRect().size.x, 
								0.5f * sporeSprite.getTextureRect().size.y });
	}

	void update(float dt = 0.001f)
	{
		static float sporeGravity = 0.4f;
		velocity.y += sporeGravity;
		sporeSprite.setPosition(velocity + sporeSprite.getPosition());
		blinking += dt;
		sporeSprite.rotate(sf::radians(dt));
		if (blinking > 2 * 3141.59f || blinking < 0.f) blinking = 0.f;
	}

	void draw(sf::RenderTarget& target)
	{
		// Will fail to draw?		
		sporeSprite.setScale({ sinf(blinking) * xScale, yScale });
		target.draw(sporeSprite, sporeShader);

		/*sf::CircleShape debugCircle(10);
		debugCircle.setOrigin({ 10.f, 10.f });
		debugCircle.setFillColor(sf::Color::Yellow);
		debugCircle.setPosition(sporeSprite.getPosition());
		target.draw(debugCircle);*/
	}

	bool operator=(const Spore& other)
	{
		// This is sooo bad I am proud of it!!
		return sporeSprite.getPosition() == other.sporeSprite.getPosition();
	}

	void reset()
	{
		visible = false;
		sporeSprite.setPosition({ 0.f, 0.f });
		velocity = sf::Vector2f{ 0.f, 0.f };
	}
};



std::list<Banana> bananas;
constexpr size_t MAX_SHROOMS = 1000;
constexpr size_t MAX_SPORES = 10 * MAX_SHROOMS;

static bool isAnyPosInHorizontalLineBelowSkyline(const sf::Vector2f& pos, float deltaX, const Level& level)
{
	for (float step = 0.f; step < deltaX; step += 0.1f)
	{
		if (level.isBelowSkyline(sf::Vector2f{ pos.x + step, pos.y }))
			return true;
	}
	return false;
}


static sf::Vector2f getShroomAtHighestYPoint(const sf::Vector2f& pos, const Level& level)
{
	sf::Vector2f shroomPos = { pos.x, pos.y };
	int maxShift = 0;
	while (isAnyPosInHorizontalLineBelowSkyline(shroomPos, 10.f, level) && maxShift++ < 1000)
		shroomPos.y -= 5.f; // = sf::Vector2f{ shroomPos.x, shroomPos.y - 5.f };
	return shroomPos;
}

uint8_t globalBananaId = 0;
int main()
{
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<> distributionBetweenZeroAndOne(0.f, 1.f);

	const uint32_t width = sf::VideoMode::getDesktopMode().size.x;
	const uint32_t height = sf::VideoMode::getDesktopMode().size.y;
	sf::RenderWindow window(sf::VideoMode({ width, height }), "Kong at size (" + std::to_string(width) + ", " + std::to_string(height) + ")");

	const uint32_t framerate = 60;
	window.setFramerateLimit(framerate);

	const auto bananaTexture = std::make_shared<sf::Texture>();
	const auto playerTexture = std::make_shared<sf::Texture>();
	const auto pSingleShroomTexture = std::make_shared<sf::Texture>();
	const auto pTwoShroomTexture = std::make_shared<sf::Texture>();
	const auto pThreeShroomTexture = std::make_shared<sf::Texture>();
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
	if (!pSingleShroomTexture->loadFromFile("assets/Shroom.png"))
	{
		std::cerr << "Failed to load shroom texture.\n";
		return -1;
	}
	if (!pTwoShroomTexture->loadFromFile("assets/TwoShroom.png"))
	{
		std::cerr << "Failed to load shroom texture.\n";
		return -1;
	}
	if (!pThreeShroomTexture->loadFromFile("assets/ThreeShroom.png"))
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
	shroomShader.setUniform("texture", *pSingleShroomTexture);
	shroomShader.setUniform("textureSize", (sf::Vector2f)pSingleShroomTexture->getSize());

	std::vector<Shroom> shrooms(MAX_SHROOMS, Shroom(sf::Vector2f{ 0.f, 0.f }, pSingleShroomTexture, 0, &shroomShader));
	

	sf::Shader bananaShader;
	if (!bananaShader.loadFromFile("assets/BananaVertexShader", "assets/BananaFragmentShader"))
	{
		std::cerr << "Failed to load banana shader!\n";
		return -1;
	}

	bananaShader.setUniform("texture", *bananaTexture);
	bananaShader.setUniform("maxExplosion", (float)framerate);
	bananaShader.setUniform("textureSize", sf::Vector2f(bananaTexture->getSize().x, bananaTexture->getSize().y));


	sf::Texture sporeTexture;
	if (!sporeTexture.loadFromFile("assets/Spore.png"))
	{
		std::cerr << "Failed to load spore texture exiting\n";
		return -1;
	}
	sf::Shader sporeShader;
	if (!sporeShader.loadFromFile("assets/SporeVertexShader", "assets/SporeFragmentShader"))
	{
		std::cerr << "Failed to load spore shader; Critical for this game exiting blyat!!\n";
		return -1;
	}

	sporeShader.setUniform("textureSize", (sf::Vector2f)sporeTexture.getSize());
	sporeShader.setUniform("texture", sporeTexture);

	std::vector<Spore> spores(MAX_SPORES, Spore(&sporeTexture, &sporeShader, sf::Vector2f{0.f, 0.f}, sf::Vector2f{ 0.f, 0.f }));

	Level level;
	const uint32_t desiredNumSkyscrapers = 30;
	// Offset of x for player on top of skyscraper. Oh blyat!!
	const float magicScalarKurwa = 0.25f;
	level.generateSkyline(width, height, desiredNumSkyscrapers);
	// TODO Broken due to scale
	std::vector<sf::Vector2f> playerPositions = level.getPlayerPositions(magicScalarKurwa);

	Player player0(playerPositions[0], playerTexture, true);
	Player player1(playerPositions[1], playerTexture, false);

	// Ensure you are not too fat in x direction to fit skyscraper bitch!
	player0.mySprite.setScale({ 20 * player0.scaleFactor / desiredNumSkyscrapers, player0.mySprite.getScale().y });
	player1.mySprite.setScale({ -20 * player1.scaleFactor / desiredNumSkyscrapers, player1.mySprite.getScale().y });

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
	float shroomingSpeed = 32;
	size_t shroomIdx = 0;
	size_t sporeIdx = 0;

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
				else if (event->code == sf::Keyboard::Key::R /*&& playerWon != -1*/)
				{
					for (int i = 0; i < MAX_SHROOMS; ++i)
						shrooms[i].reset();
					shroomIdx = 0;
					for (int i = 0; i < MAX_SPORES; ++i)
						spores[i].reset();
					sporeIdx = 0;

					for (auto bit = bananas.begin(); bit != bananas.end();)
						bit = bananas.erase(bit);
					level.generateSkyline(width, height, desiredNumSkyscrapers);
					// TODO Broken
					auto newPositions = level.getPlayerPositions(magicScalarKurwa);
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
							/*banana.debug = !level.isBelowSkyline(pos);
							if (banana.debug && playerToThrow) player1.debug = true;
							else if (banana.debug && !playerToThrow) player0.debug = true;
							else player0.debug = player1.debug = false;*/
							player0Health.setString(std::to_string(player0.health));
							player1Health.setString(std::to_string(player1.health));
							shouldExplode = true;
							break;
						}
					}
				}
				if (shouldExplode) banana.explodingFrame++;

				banana.draw(window, sf::RenderStates::Default);

				if (banana.explodingFrame > framerate/* || isOutsideOfWindow(window, banana.getPosition())*/)
				{
					auto shroomPos = getShroomAtHighestYPoint(banana.getPosition(), level);
					shrooms[shroomIdx] = Shroom(sf::Vector2f{shroomPos.x, shroomPos.y - 25.f}, pSingleShroomTexture, 1, &shroomShader);
					shrooms[shroomIdx].visible = true;
					shroomIdx++;
					//shrooms.back().debug = true;
					it = bananas.erase(it);
					/*if (player0.health <= 0) playerWon = 1;
					else if (player1.health <= 0) playerWon = 0;*/
				}
				else
					++it;
			}


			// Deal with shrooms, make hallucinations bloom
			float splitShrooms = 0.2f;
			if (shroomIdx > 2)
				shroomingSpeed = std::min(3.f, shroomingSpeed - 0.001f);
			for (int shroomAccessIdx = 0; shroomAccessIdx < MAX_SHROOMS; ++shroomAccessIdx)
			{
				auto& shroom = shrooms[shroomAccessIdx];
				if (shroom.visible)
				{
					shroom.update();
					// TODO Shroom lifecycle shit. Dying after sporing good but still...
					if (shroom.age < 10 * splitShrooms * framerate)
					{
						shroom.shader->setUniform("texture", *pSingleShroomTexture);
					}
					else if (shroom.age > 10 * splitShrooms * framerate && shroom.age <= 20 * splitShrooms * framerate)
					{
						shroom.shader->setUniform("texture", *pTwoShroomTexture);
						shroom.mySprite.setScale({ 1.002f * shroom.mySprite.getScale().x, 1.002f * shroom.mySprite.getScale().y });
					}
					else if (shroom.age > 20 * splitShrooms * framerate)
					{
						shroom.shader->setUniform("texture", *pThreeShroomTexture);
					}
					else if (shroom.age > 30 * splitShrooms * framerate)
					{
						shroom.visible = false;
						continue;
					}
					if (shroom.sporeReleaseCounter > shroomingSpeed * framerate)
					{
						for (int i = 0; i < 10; ++i)
						{
							float rand = 3.14159f * distributionBetweenZeroAndOne(generator);
							spores[sporeIdx].sporeSprite.setPosition(sf::Vector2f{ shroom.mySprite.getPosition().x, 2.9f * shroom.mySprite.getPosition().y / 3 });
							spores[sporeIdx].velocity = sf::Vector2f{ 7.5f * cosf(rand), -15.f * sinf(rand) };
							spores[sporeIdx].visible = true;
							sporeIdx++;
							if (sporeIdx > MAX_SPORES) sporeIdx = 0;
						}
						// TODO Add reset functionality on all entities!!
						shroom.sporeReleaseCounter = 0;
						// Hmmm....?!
						// AHA! Erasing actually does not work... CPP is insane... fuck :D
						// Use C-style arrays soon...blyat!
						shroom.visible = false;
						continue;
					}
					shroom.draw(window, sf::RenderStates::Default);				
				}
			}

			// tEh Spores are coming for you!!
			for (size_t localSporeIdx = 0; localSporeIdx < MAX_SPORES; ++localSporeIdx)
			{
				auto& spore = spores[localSporeIdx];
				if (spore.visible)
				{
					spore.update(0.5f);
					auto sporePos = spore.sporeSprite.getPosition();
					if (level.isBelowSkyline(sporePos))
					{
						bool closeToOtherShroom = false;
						for (int i = 0; i < MAX_SHROOMS; ++i)
						{
							if (shrooms[i].visible && abs(shrooms[i].mySprite.getPosition().x - sporePos.x) < 10.f)
							{
								closeToOtherShroom = true;
								break;
							}
						}
						if (!closeToOtherShroom && sporePos.y < height / 2)
						{
							float rand = distributionBetweenZeroAndOne(generator);
							// 20% chance of new shroom kurwa!!
							if (rand > 0.8)
							{
								auto newShroomPos = getShroomAtHighestYPoint(sporePos, level);
								shrooms[shroomIdx] = Shroom(newShroomPos, pSingleShroomTexture, 1, &shroomShader);
								shrooms[shroomIdx].visible = true;
								// TODO Crash because out of range but also need to vectorize spores as well as rest of entities!!
								// Arenas all the way!
								shroomIdx++;
							}
						}
						spore.visible = false;
						continue;
					}
					spore.draw(window);					

				}
			}



			sf::Text debugText(font, "Shroom idx kurwa: " + std::to_string(shroomIdx) + "\nBanana size kurwa " + std::to_string(bananas.size()));
			debugText.setPosition({ 3.f * width / 7, height / 16.f });
			window.draw(debugText);
		}
		else
		{
			sf::Text winningText(font, "Winning player ID " + std::to_string(playerWon) + "\nPress R to reset kurwa!");
			winningText.setPosition({ 3.f * width / 7, height / 4.f });
			window.draw(winningText);
		}

		window.display();
	}
}