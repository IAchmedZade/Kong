#include "Player.h"

#include <sfml/Graphics/RenderTarget.hpp>


Player::Player(sf::Vector2f position, std::shared_ptr<sf::Texture> texture)
	: pMyTexture(texture)
	, mySprite(*texture)
{
	mySprite.setTexture(*pMyTexture);
	mySprite.setPosition(position);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mySprite, states);
}
