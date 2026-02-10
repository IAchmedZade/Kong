#include "Banana.h"

#include <sfml/Graphics/RenderTarget.hpp>
#include <sfml/graphics.hpp>
#include <vector>



Banana::Banana(sf::Vector2f position, std::shared_ptr<sf::Texture> texture, uint8_t id)
	: pMyTexture(texture)
	, mySprite(*texture)
	, id(id)
{

	mySprite.setTexture(*pMyTexture);
	mySprite.setPosition(position);
	mySprite.setOrigin((sf::Vector2f)mySprite.getTextureRect().getCenter());
	mySprite.setScale({ 0.5f, 0.5f });
}

void Banana::update()
{
	if (!explodingFrame)
	{
		constexpr float gravity = .81f;

		myVelocity.y += gravity;

		mySprite.move(myVelocity);
		mySprite.rotate(sf::degrees(20.0f));
	}	
}

void Banana::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::CircleShape explosion(explodingFrame);
	explosion.setFillColor(sf::Color::Red);
	explosion.setOrigin({ (float) explodingFrame, (float) explodingFrame }); 
	explosion.setPosition(mySprite.getPosition());

	target.draw(explosion);
	target.draw(mySprite, states);
}

std::vector<sf::Vector2f> Banana::getShittyBoundingPixels() const
{
	// Stupid circle need to do parabolas blyat!
	const size_t numPixels = 100;
	std::vector<sf::Vector2f> boundingPixels(numPixels, {0.f ,0.f});
	const float radius = 10.f; // sqrtf(mySprite.getTextureRect().size.x * mySprite.getTextureRect().size.x / 6		+ mySprite.getTextureRect().size.y * mySprite.getTextureRect().size.y / 6);
	
	const sf::Vector2f spriteOrigin = mySprite.getPosition();
	for (size_t i = 0; i < 100; ++i)
	{
		boundingPixels[i] = {
			spriteOrigin.x + radius * cosf(2 * 3.14159 * i / numPixels),
			spriteOrigin.y + radius * sinf(2 * 3.14159 * i / numPixels)
		};
	}
	return boundingPixels;
}

