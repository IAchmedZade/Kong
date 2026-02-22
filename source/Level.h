#pragma once
#include <vector>
#include <SFML/Graphics.hpp>


class Level final
{
public:
	std::vector<sf::RectangleShape> mySkyline;
	// generate level funtion here
	void generateSkyline(const uint32_t width, const uint32_t height, const uint32_t minWidthSkyscraper);
	bool isBelowSkyline(const sf::Vector2f& pos);
	std::vector<sf::Vector2f> getPlayerPositions(float magicScalar = 0.25f) const;
	void draw(sf::RenderTarget& target);
};
