#include "Bird.h"
#include "Globals.h"
#include <stdexcept>

Bird::Bird() :
	gravity(900.f),
	flap_speed(330.f),
	velocity_y(0.f),
	anim_timer(0.f),
	texture_switch(1),
	should_fly(false),
	on_ground(false),
	bird_sprite(textures[0])          // SFML 3: a Sprite cannot exist without a texture
{
	if (!textures[0].loadFromFile("assets/birddown.png") ||
		!textures[1].loadFromFile("assets/birdup.png"))
		throw std::runtime_error("Could not load bird textures (check the assets folder / working directory)");

	// 'true' = reset the texture rectangle. The sprite was built with an EMPTY
	// texture, so without this it would have a 0x0 rectangle and be invisible.
	bird_sprite.setTexture(textures[0], true);
	bird_sprite.setScale({ SCALE_FACTOR, SCALE_FACTOR });
	resetBirdPosition();
}

void Bird::update(sf::Time& dt)
{
	if (!should_fly) return;
	const float secs = dt.asSeconds();

	// Wing animation: swap texture every 0.12s (independent of frame rate)
	anim_timer += secs;
	if (anim_timer >= 0.12f)
	{
		anim_timer = 0.f;
		texture_switch = 1 - texture_switch;
		bird_sprite.setTexture(textures[texture_switch]);   // same size, rect stays valid
	}

	// Gravity
	velocity_y += gravity * secs;
	bird_sprite.move({ 0.f, velocity_y * secs });

	const sf::FloatRect b = bird_sprite.getGlobalBounds();

	if (b.position.y < 0.f)                              // ceiling
	{
		bird_sprite.setPosition({ bird_sprite.getPosition().x, 0.f });
		velocity_y = 0.f;
	}
	else if (b.position.y + b.size.y >= GROUND_Y)        // ground
	{
		bird_sprite.setPosition({ bird_sprite.getPosition().x, GROUND_Y - b.size.y });
		velocity_y = 0.f;
		should_fly = false;
		on_ground = true;
	}
}

void Bird::flapBird()
{
	velocity_y = -flap_speed;
}

float Bird::getRightBound()
{
	const sf::FloatRect b = bird_sprite.getGlobalBounds();
	return b.position.x + b.size.x;
}

void Bird::resetBirdPosition()
{
	bird_sprite.setPosition({ 100.f, 50.f });
	bird_sprite.setTexture(textures[0]);
	texture_switch = 1;
	anim_timer = 0.f;
	velocity_y = 0.f;
	on_ground = false;
}

void Bird::setShouldFly(bool should_fly)
{
	this->should_fly = should_fly;
}

bool Bird::hasHitGround() const
{
	return on_ground;
}

sf::FloatRect Bird::getHitbox() const
{
	sf::FloatRect b = bird_sprite.getGlobalBounds();
	const float shrink = 5.f;
	return sf::FloatRect({ b.position.x + shrink, b.position.y + shrink },
		{ b.size.x - 2.f * shrink, b.size.y - 2.f * shrink });
}