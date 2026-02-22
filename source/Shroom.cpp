#include "Shroom.h"

#include <sfml/Graphics/RenderTarget.hpp>
#include <sfml/graphics.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <vector>

Shroom::Shroom(sf::Vector2f position, std::shared_ptr<sf::Texture> texture, uint8_t sporeReleaseCounter, sf::Shader* shader)
	: pMyTexture(texture)
	, mySprite(*texture)
	, sporeReleaseCounter(sporeReleaseCounter)
	, shader(shader)
{

	mySprite.setTexture(*pMyTexture);
	mySprite.setPosition(position);
	mySprite.setOrigin((sf::Vector2f)mySprite.getTextureRect().getCenter());
	mySprite.setScale({ 0.15f, 0.15f });

}

void Shroom::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	static float debugFloat = 0.f;
	debugFloat += 0.01f;
	shader->setUniform("blinking", debugFloat);
	states.shader = shader;
	target.draw(mySprite, states);
	if (debug)
	{
		sf::CircleShape debug(10);
		debug.setFillColor(sf::Color::Red);
		debug.setOrigin({ 10,10 });
		debug.setPosition(mySprite.getPosition());
		target.draw(debug);
	}
}


void Shroom::update()
{
	sporeReleaseCounter++;
	age++;	
}