#include "Player.h"

#include <sfml/Graphics/RenderTarget.hpp>


Player::Player(sf::Vector2f position, std::shared_ptr<sf::Texture> texture, bool left)
	: pMyTexture(texture)
	, mySprite(*texture)
{
	mySprite.setTexture(*pMyTexture);
	mySprite.setPosition({ position.x , position.y - scaleFactor *  mySprite.getTextureRect().size.y});
	mySprite.setScale({ left ? scaleFactor : -scaleFactor, scaleFactor });
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mySprite, states);
}

void Player::setPosition(const sf::Vector2f& pos)
{
	mySprite.setPosition({ pos.x , pos.y - scaleFactor * mySprite.getTextureRect().size.y });
}

bool Player::checkIfHitAndDecrementHealth(const sf::Vector2f& pos)
{
	// Ellipsoid kurwa
	// (x/a)^2 + (y/b)^2 = 1
	// a = x_t, b = y_t for texture size = (x_t, y_t)
	// BROKEN!
	const float x = pos.x - mySprite.getPosition().x;
	const float y = pos.y - mySprite.getPosition().y;
	const float x_t = mySprite.getTextureRect().size.x / scaleFactor;
	const float y_t = mySprite.getTextureRect().size.y / scaleFactor;
	bool isHit = (x / x_t) * (x / x_t) + (y / y_t) * (y / y_t) < 1;
	if (isHit)
		myHealth--;
	return isHit;
}