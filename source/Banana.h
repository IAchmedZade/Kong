#pragma once

#include <sfml/Graphics/Sprite.hpp>
#include <sfml/System/Vector2.hpp>

#include <memory>


class Banana final : public sf::Drawable
{
public:
	Banana(sf::Vector2f position, std::shared_ptr<sf::Texture> texture);

	void update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setVelocity(sf::Vector2f velocity) { myVelocity = velocity; }
	auto getVelocity() const { return myVelocity; }

private:
	std::shared_ptr<sf::Texture> pMyTexture;

	sf::Vector2f myVelocity;
	sf::Sprite mySprite;
};
