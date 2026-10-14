#pragma once
#include <SFML/Graphics.hpp>

class Pipe
{
public:
	explicit Pipe(float y_pos);        // y_pos = top edge of the lower pipe

	sf::Sprite sprite_up, sprite_down; // up = lower pipe, down = upper pipe
	bool scored = false;

	static void loadTextures();        // call once BEFORE creating any Pipe
	void  update(sf::Time&);
	float getRightBound() const;

private:
	static sf::Texture texture_down, texture_up;
	static float pipe_distance, move_speed;
};