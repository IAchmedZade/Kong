#pragma once

#include <sfml/Graphics/Sprite.hpp>

#include <memory>


class Player final : public sf::Drawable
{
public:
	Player(sf::Vector2f position, std::shared_ptr<sf::Texture> texture);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	auto getPosition() const { return mySprite.getPosition(); }

private:
	int myHealth = 100;

	std::shared_ptr<sf::Texture> pMyTexture;
	sf::Sprite mySprite;
};
