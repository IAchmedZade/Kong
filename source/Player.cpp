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