#include <sfml/network.hpp>
#include <sfml/window.hpp>
#include <sfml/graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <thread>
#include <chrono>

#include "Banana.h"

std::random_device rd;  // Will be used to obtain a seed for the random number engine
std::mt19937 generator(rd()); // Standard mersenne_twister_engine seeded with rd()
const float minWidthSkyscraper = 0.1;
std::uniform_real_distribution<> distributionBetweenTwentyAndFortyPercent(minWidthSkyscraper,2 * minWidthSkyscraper);

template<class T>
void printVector(const std::vector<T>& vec)
{
	for (int i : vec) std::cout << i << " ";
	std::cout << '\n';
}

std::vector<uint32_t> generateSkyline(const uint32_t width, const uint32_t height)
{
	std::vector<int> heightsAndWidths;
	std::vector<uint32_t> rands;
	uint32_t totalSum = 0;
	for (int i = 0; i < 20; ++i)
	{
		uint32_t randomWidth = (uint32_t) (width * distributionBetweenTwentyAndFortyPercent(generator));
		if (totalSum + randomWidth < width)
		{
			rands.push_back(randomWidth);
			totalSum += randomWidth;
		}
		else
		{			
			if (float(width - totalSum) / width < minWidthSkyscraper)
				rands[rands.size() - 1] += (width - totalSum);
			else
				rands.push_back(width - totalSum);
			break;
		}
	}
	std::cout << "Asserting that sum of widths equals width:\n";
	uint32_t sum = 0;
	for (int w : rands) sum += w;
	std::cout << "Width " << width << " " << sum << '\n';
	printVector(rands);
	return rands;
}

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
	sf::RectangleShape rect({ 100,100 });
	rect.setFillColor(sf::Color::Green);
	rect.setOrigin({ 50, 50 });
	rect.setPosition({ width / 4, height / 2 });


	sf::RectangleShape rect2({ 100,100 });
	rect2.setFillColor(sf::Color::Red);
	rect2.setOrigin({ 50, 50 });
	rect2.setPosition({ 3 * width / 4, height / 2 });
	std::vector<uint32_t> widths = generateSkyline(width, height);
	std::vector<sf::RectangleShape> skyline;
	uint32_t offset = 0;
	for (auto width : widths)
	{
		sf::RectangleShape box({ (float)width, 0.8 * height });
		box.setOutlineThickness(0.8f);
		box.setFillColor(sf::Color::Black);
		box.setOutlineColor(sf::Color::White);
		box.setPosition({ (float) offset, 0.2 * height });
		offset += width;
		skyline.push_back(box);
	}

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
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
		{
			widths = generateSkyline(width, height);
			skyline.clear();
			offset = 0;
			for (auto width : widths)
			{
				sf::RectangleShape box({ (float)width, 0.8 * height });
				box.setOutlineThickness(0.8f);
				box.setFillColor(sf::Color::Black);
				box.setOutlineColor(sf::Color::White);
				box.setPosition({ (float)offset, 0.2 * height });
				offset += width;
				skyline.push_back(box);
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