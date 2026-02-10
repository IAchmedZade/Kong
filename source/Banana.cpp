#include "Banana.h"

#include <sfml/Graphics/RenderTarget.hpp>
#include <vector>



Banana::Banana(sf::Vector2f position, std::shared_ptr<sf::Texture> texture)
	: pMyTexture(texture)
	, mySprite(*texture)
{

	mySprite.setTexture(*pMyTexture);
	mySprite.setPosition(position);
	mySprite.setOrigin((sf::Vector2f)mySprite.getTextureRect().getCenter());
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

std::vector<sf::Vector2f> Banana::getShittyBoundingPixels() const
{
	mySprite.getRotation();
	return {};
}

