#pragma once


#include <sfml/Graphics/Sprite.hpp>
#include <sfml/System/Vector2.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <memory>
#include <vector>

#include "Banana.h"


class Shroom final : public sf::Drawable
{
public:
	Shroom(sf::Vector2f position, std::shared_ptr<sf::Texture> texture, uint8_t id, sf::Shader* shader);

	void update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	std::shared_ptr<sf::Texture> pMyTexture;
	sf::Sprite mySprite;
	sf::Shader* shader;
	uint8_t sporeReleaseCounter;
	std::vector<Banana> bananaSpores;
};
