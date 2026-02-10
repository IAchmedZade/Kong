#include "Level.h"
#include <random>
#include <iostream>
#include <vector>



template<class T>
void printVector(const std::vector<T>& vec)
{
	for (int i : vec) std::cout << i << " ";
	std::cout << '\n';
}

std::random_device rd;  // Will be used to obtain a seed for the random number engine
std::mt19937 generator(rd()); // Standard mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<> distributionBetweenSixtyAndEightyPercent(0.4, 0.8);


// Implementation here
std::vector<sf::RectangleShape> Level::generateSkyline(const uint32_t width, const uint32_t height, const uint32_t desiredNumberOfTiles)
{
	const float inverseOfNumberOfTiles = 1 / (float)desiredNumberOfTiles;
	std::uniform_real_distribution<> distributionBetweenTwentyAndFortyPercent(inverseOfNumberOfTiles / 2, 3 * inverseOfNumberOfTiles / 2);
	std::vector<int> heightsAndWidths;
	
	std::vector<sf::RectangleShape> skyscrapers;
	uint32_t totalSum = 0;
	uint32_t offset = 0;
	sf::RectangleShape box({0,0 });
	box.setOutlineThickness(1.8f);
	box.setFillColor(sf::Color::Yellow);
	box.setOutlineColor(sf::Color::White);
	box.setOrigin({ 0,0 });
	
	for (int i = 0; i < desiredNumberOfTiles; ++i)
	{
		uint32_t randomWidth = (uint32_t)(width * distributionBetweenTwentyAndFortyPercent(generator));
		if (totalSum + randomWidth < width)
		{
			totalSum += randomWidth;
			const float skyscraperHeight = distributionBetweenSixtyAndEightyPercent(generator);
			box.setSize({ (float)randomWidth, skyscraperHeight * height });
			box.setPosition({ (float)offset, (1 - skyscraperHeight) * height });
			offset += randomWidth;
			skyscrapers.push_back(box);
		}
		else
		{
			std::cout << "Offset " << offset << " random width " << randomWidth << "\nwidth - offset " << width - offset << '\n';
			const float skyscraperHeight = distributionBetweenSixtyAndEightyPercent(generator);
			box.setSize({ (float)(width - offset) - 1.f, 1.0f * height });
			box.setPosition({ (float)offset,  0.f * height });
			std::cout << "Box dimensions " << box.getSize().x << " " << box.getSize().y << " and position " <<
				box.getPosition().x << " " << box.getPosition().y << '\n';
			skyscrapers.push_back(box);
			break;
		}
	}
	return skyscrapers;
}