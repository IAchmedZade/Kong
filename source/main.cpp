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


int main()
{
	const uint32_t width = 800;
	const uint32_t height = 600;
	sf::RenderWindow window(sf::VideoMode({ width, height }), "Kong");
	sf::RectangleShape rect({ 100,100 });
	rect.setFillColor(sf::Color::Green);
	rect.setOrigin({ 50, 50 });
	rect.setPosition({ width / 4, height / 2 });


	sf::RectangleShape rect2({ 100,100 });
	rect2.setFillColor(sf::Color::Green);
	rect2.setOrigin({ 50, 50 });
	rect2.setPosition({ 3 * width / 4, height / 2 });


	while (window.isOpen())
	{
		window.clear();
		window.draw(rect);
		window.draw(rect2);
		window.display();
	}
}