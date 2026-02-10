#include "Banana.h"

#include <sfml/Graphics/RenderTarget.hpp>


Banana::Banana(sf::Vector2f position, std::shared_ptr<sf::Texture> texture)
	: pMyTexture(texture)
	, mySprite(*texture)
{

	mySprite.setTexture(*pMyTexture);
	mySprite.setPosition(position);
}

void Banana::update()
{
	constexpr float gravity = 0.03f;

	myVelocity.y += gravity;

	mySprite.move(myVelocity);
	mySprite.rotate(sf::degrees(2.0f));
}

void Banana::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mySprite, states);
}
