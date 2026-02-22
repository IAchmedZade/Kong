#pragma once

#include <sfml/Graphics/Sprite.hpp>
#include <sfml/System/Vector2.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <memory>
#include <vector>


class Banana final : public sf::Drawable
{
public:
	Banana(sf::Vector2f position, std::shared_ptr<sf::Texture> texture, uint8_t id, sf::Shader* shader);

	void update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;	

	void setVelocity(sf::Vector2f velocity) { myVelocity = velocity; }
	auto getVelocity() const { return myVelocity; }
	sf::Vector2f getPosition() const { return mySprite.getPosition(); }
	std::vector<sf::Vector2f> getShittyBoundingPixels() const;

	uint8_t explodingFrame = 0;
	uint8_t id = 0;
	bool debug = false;


private:
	std::shared_ptr<sf::Texture> pMyTexture;

	sf::Vector2f myVelocity;
	sf::Sprite mySprite;
	sf::Shader* shader;
};
