#include "Pipe.h"
#include "Globals.h"
#include <stdexcept>

sf::Texture Pipe::texture_down, Pipe::texture_up;
float Pipe::pipe_distance = 170.f;   // gap between top and bottom pipe
float Pipe::move_speed = 250.f;   // same speed as the ground

Pipe::Pipe(float y_pos) :
	sprite_up(texture_up),
	sprite_down(texture_down)
{
	sprite_up.setScale({ SCALE_FACTOR, SCALE_FACTOR });
	sprite_down.setScale({ SCALE_FACTOR, SCALE_FACTOR });

	const float x = static_cast<float>(WIN_WIDTH);
	sprite_up.setPosition({ x, y_pos });
	sprite_down.setPosition({ x, y_pos - pipe_distance - sprite_up.getGlobalBounds().size.y });
}

void Pipe::update(sf::Time& dt)
{
	const float dx = -move_speed * dt.asSeconds();
	sprite_up.move({ dx, 0.f });
	sprite_down.move({ dx, 0.f });
}

void Pipe::loadTextures()
{
	if (!texture_down.loadFromFile("assets/pipedown.png") ||
		!texture_up.loadFromFile("assets/pipe.png"))
		throw std::runtime_error("Could not load pipe textures (check the assets folder / working directory)");
}

float Pipe::getRightBound() const
{
	const sf::FloatRect b = sprite_up.getGlobalBounds();
	return b.position.x + b.size.x;
}