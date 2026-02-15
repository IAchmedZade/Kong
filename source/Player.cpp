#include "Player.h"

#include <sfml/Graphics/RenderTarget.hpp>
#include <SFML/Graphics.hpp>


Player::Player(sf::Vector2f position, std::shared_ptr<sf::Texture> texture, bool left)
	: pMyTexture(texture)
	, mySprite(*texture)
	, left(left)
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

bool Player::checkIfHitAndDecrementHealth(const sf::Vector2f& pos, sf::RenderTarget& target)
{
	const float yDiff = pos.y - mySprite.getPosition().y;
	const float xDiff = pos.x - mySprite.getPosition().x;
	bool isHit = (yDiff < mySprite.getTextureRect().size.y * scaleFactor && yDiff > 0
		&& abs(xDiff) < mySprite.getTextureRect().size.x * scaleFactor);
	if (isHit)	health-= 100;
	return isHit;
}