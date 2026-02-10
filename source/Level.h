#pragma once
#include <vector>
#include <SFML/Graphics.hpp>


class Level final
{
public:
	// generate level funtion here
	std::vector<sf::RectangleShape> generateSkyline(const uint32_t width, const uint32_t height, const uint32_t minWidthSkyscraper);
};
