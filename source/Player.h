#pragma once

#include <sfml/Graphics/Sprite.hpp>

#include <memory>


class Player final : public sf::Drawable
{
public:
	Player(sf::Vector2f position, std::shared_ptr<sf::Texture> texture, bool left);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	auto getPosition() const { return mySprite.getPosition(); }
	void setPosition(const sf::Vector2f& pos);
	bool checkIfHitAndDecrementHealth(const sf::Vector2f& pos, sf::RenderTarget& target);
	int getHealth() { return myHealth; }
	
private:
	int myHealth = 100;
	bool left;
	const float scaleFactor = 0.5f;
	std::shared_ptr<sf::Texture> pMyTexture;
	sf::Sprite mySprite;
};
