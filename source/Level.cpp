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
std::uniform_real_distribution<> distributionForHeight(0.4, 0.5);


// Implementation here
void Level::generateSkyline(const uint32_t width, const uint32_t height, const uint32_t desiredNumberOfTiles)
{
	const float inverseOfNumberOfTiles = 1 / (float)desiredNumberOfTiles;
	std::uniform_real_distribution<> distributionBetweenTwentyAndFortyPercent(inverseOfNumberOfTiles / 2, 3 * inverseOfNumberOfTiles / 2);
	std::vector<int> heightsAndWidths;
	
	std::vector<sf::RectangleShape> skyscrapers;
	uint32_t totalSum = 0;
	uint32_t offset = 0;
	sf::RectangleShape box({0,0 });
	box.setOutlineThickness(1.8f);
	box.setFillColor(sf::Color(42, 42, 42));
	box.setOutlineColor(sf::Color::White);
	box.setOrigin({ 0,0 });
		
	while(1)
	{
		uint32_t randomWidth = width / desiredNumberOfTiles; //(uint32_t)(width * distributionBetweenTwentyAndFortyPercent(generator));
		if (totalSum + randomWidth < width)
		{
			totalSum += randomWidth;
			const float skyscraperHeight = distributionForHeight(generator);
			box.setSize({ (float)randomWidth, skyscraperHeight * height });
			box.setPosition({ (float)offset, (1 - skyscraperHeight) * height });
			offset += randomWidth;
			skyscrapers.push_back(box);
		}
		else
		{
			if ((float)(width - totalSum) < 0.05 * width)
			{
				skyscrapers.back().setSize({ skyscrapers.back().getSize().x + width - totalSum,
					skyscrapers.back().getSize().y });
			}
			else
			{
				const float skyscraperHeight = distributionForHeight(generator);
				box.setSize({ (float)(width - offset), skyscraperHeight * height });
				box.setPosition({ (float)offset,  (1.f - skyscraperHeight) * height });
				skyscrapers.push_back(box);
			}
			break;
			
		}
	}
	this->mySkyline.clear();
	this->mySkyline = skyscrapers;
}


bool Level::isBelowSkyline(const sf::Vector2f& pos)
{
	for (auto& rect : mySkyline)
	{
		const sf::Vector2f rectPos = rect.getPosition();
		const sf::Vector2f rectSize = rect.getSize();
		if (rectPos.x <= pos.x && rectPos.x + rectSize.x > pos.x
			&& rectPos.y >= pos.y)
			return false;
	}
	return true;
}


void Level::draw(sf::RenderTarget & target)
{
	for (auto& box : mySkyline) target.draw(box);
}


std::vector<sf::Vector2f> Level::getPlayerPositions() const
{
	if (mySkyline.size() > 3)
	{
		// TODO Make random appearance? 
		// Allow movement?
		auto& firstBox = mySkyline[1];
		auto& secondBox = mySkyline[mySkyline.size() - 2];
		const float magicXOffset = 0.25f;
		return 
		{
			{firstBox.getPosition().x + magicXOffset * firstBox.getSize().x , firstBox.getPosition().y},			
			{secondBox.getPosition().x + (1 - magicXOffset) * secondBox.getSize().x , secondBox.getPosition().y}
		};
	}
	// BAD!! Oh kurwa! Oh Bober!
	return { {0,0}, {0,0} };
}
