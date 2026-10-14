#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include <string>
#include "Bird.h"
#include "Pipe.h"

class Game
{
public:
	explicit Game(sf::RenderWindow&);
	void startGameLoop();

private:
	// NOTE: member order matters - textures/font must exist before the
	// sprites/texts that use them are constructed.
	sf::RenderWindow& win;
	sf::Texture bg_texture, ground_texture;
	sf::Font    font;
	sf::Sprite  bg_sprite, ground_sprite1, ground_sprite2;
	Bird        bird;
	sf::Text    score_text, hint_text, game_over_text, restart_text;

	bool  is_enter_pressed, run_game;
	int   score;
	float spawn_timer;
	const float spawn_interval = 1.5f;     // seconds between pipes
	const float move_speed = 250.f;    // ground speed (px/s), same as pipes

	std::vector<Pipe> pipes;
	std::mt19937 rng;
	std::uniform_int_distribution<int> dist{ 250, 550 };
	sf::SoundBuffer flap_buffer, score_buffer, dead_buffer;   
	sf::Sound flap_sound, score_sound, dead_sound;

	void handleEvents();
	void doProcessing(sf::Time& dt);
	void moveGround(sf::Time&);
	void checkCollisions();
	void checkScore();
	void restartGame();
	void draw();
	static void centerText(sf::Text& text, float x, float y);
};