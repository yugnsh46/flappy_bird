#pragma once
#include <SFML/Graphics.hpp>

class Bird
{
private:
	sf::Texture textures[2];      // declared BEFORE bird_sprite on purpose
	const float gravity;          // pixels / second^2
	const float flap_speed;       // pixels / second (upward)
	float velocity_y;             // pixels / second
	float anim_timer;
	int   texture_switch;
	bool  should_fly;
	bool  on_ground;

public:
	sf::Sprite bird_sprite;

	Bird();
	void  setShouldFly(bool);
	void  flapBird();
	void  resetBirdPosition();
	void  update(sf::Time&);
	float getRightBound();
	bool  hasHitGround() const;
	sf::FloatRect getHitbox() const;   // slightly smaller than the sprite = fairer
};
