#include "Game.h"
#include "Globals.h"
#include <algorithm>

Game::Game(sf::RenderWindow& window) :
	win(window),
	bg_texture("assets/bg.png"),            // SFML 3: texture/font constructors load the file
	ground_texture("assets/ground.png"),
	font("assets/arial.ttf"),
	bg_sprite(bg_texture),
	ground_sprite1(ground_texture),
	ground_sprite2(ground_texture),
	score_text(font, "Score: 0", 28),
	hint_text(font, "Press ENTER to start, SPACE to flap", 24),
	game_over_text(font, "GAME OVER", 56),
	restart_text(font, "Restart Game!!", 40),
	is_enter_pressed(false),
	run_game(true),
	score(0),
	spawn_timer(0.f),
	rng(std::random_device{}()),
	flap_buffer("assets/sfx/flap.wav"),
	score_buffer("assets/sfx/score.wav"),
	dead_buffer("assets/sfx/dead.wav"),
	flap_sound(flap_buffer),
	score_sound(score_buffer),
	dead_sound(dead_buffer)
{
	win.setFramerateLimit(60);
	win.setKeyRepeatEnabled(false);

	// Background
	bg_sprite.setScale({ SCALE_FACTOR, SCALE_FACTOR });
	bg_sprite.setPosition({ 0.f, -250.f });

	// Ground (two sprites scrolling side by side)
	ground_sprite1.setScale({ SCALE_FACTOR, SCALE_FACTOR });
	ground_sprite2.setScale({ SCALE_FACTOR, SCALE_FACTOR });
	ground_sprite1.setPosition({ 0.f, GROUND_Y });
	ground_sprite2.setPosition({ ground_sprite1.getGlobalBounds().size.x, GROUND_Y });

	// Texts
	score_text.setFillColor(sf::Color::Black);
	score_text.setPosition({ 15.f, 15.f });

	hint_text.setFillColor(sf::Color::Black);
	centerText(hint_text, WIN_WIDTH / 2.f, 330.f);

	game_over_text.setFillColor(sf::Color::Red);
	game_over_text.setOutlineColor(sf::Color::Black);
	game_over_text.setOutlineThickness(3.f);
	centerText(game_over_text, WIN_WIDTH / 2.f, 260.f);

	restart_text.setFillColor(sf::Color::Black);
	centerText(restart_text, WIN_WIDTH / 2.f, 680.f);

	Pipe::loadTextures();
}

void Game::centerText(sf::Text& text, float x, float y)
{
	const sf::FloatRect b = text.getLocalBounds();
	text.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
	text.setPosition({ x, y });
}

void Game::startGameLoop()
{
	sf::Clock clock;
	while (win.isOpen())
	{
		sf::Time dt = clock.restart();
		if (dt.asSeconds() > 0.05f) dt = sf::seconds(0.05f);   // avoid huge jumps (window drag etc.)

		handleEvents();
		doProcessing(dt);
		draw();
		win.display();
	}
}

void Game::handleEvents()
{
	while (const std::optional event = win.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			win.close();
		}
		else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
		{
			if (key->code == sf::Keyboard::Key::Escape)
			{
				win.close();
			}
			else if (run_game)
			{
				if (key->code == sf::Keyboard::Key::Enter && !is_enter_pressed)
				{
					is_enter_pressed = true;
					bird.setShouldFly(true);
				}
				else if (key->code == sf::Keyboard::Key::Space && is_enter_pressed)
				{
					bird.flapBird();
					flap_sound.play();
				}
			}
			else if (key->code == sf::Keyboard::Key::Enter)
			{
				restartGame();     // Enter also restarts after game over
			}
		}
		else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
		{
			if (mouse->button == sf::Mouse::Button::Left && !run_game)
			{
				const sf::Vector2f p = win.mapPixelToCoords(mouse->position);
				if (restart_text.getGlobalBounds().contains(p))
					restartGame();
			}
		}
	}
}

void Game::doProcessing(sf::Time& dt)
{
	if (is_enter_pressed)
	{
		moveGround(dt);

		// Spawn pipes on a timer
		spawn_timer += dt.asSeconds();
		if (spawn_timer >= spawn_interval || pipes.empty())
		{
			pipes.emplace_back(static_cast<float>(dist(rng)));
			spawn_timer = 0.f;
		}

		for (Pipe& pipe : pipes)
			pipe.update(dt);

		// Remove pipes that left the screen
		pipes.erase(std::remove_if(pipes.begin(), pipes.end(),
			[](const Pipe& p) { return p.getRightBound() < 0.f; }), pipes.end());
	}

	bird.update(dt);       // keeps falling after death until it reaches the ground

	if (is_enter_pressed)
	{
		checkCollisions();
		checkScore();
	}
}

void Game::checkCollisions()
{
	const sf::FloatRect hb = bird.getHitbox();
	bool hit = bird.hasHitGround();

	for (const Pipe& pipe : pipes)
	{
		if (hb.findIntersection(pipe.sprite_up.getGlobalBounds()) ||
			hb.findIntersection(pipe.sprite_down.getGlobalBounds()))
		{
			hit = true;
			break;
		}
	}

	if (hit)
	{
		dead_sound.play();
		is_enter_pressed = false;   // freeze ground + pipes
		run_game = false;           // show game over
	}
}

void Game::checkScore()
{
	const float bird_left = bird.bird_sprite.getGlobalBounds().position.x;
	for (Pipe& pipe : pipes)
	{
		if (!pipe.scored && pipe.getRightBound() < bird_left)
		{
			pipe.scored = true;
			score++;
			score_sound.play();
			score_text.setString("Score: " + std::to_string(score));
		}
	}
}

void Game::moveGround(sf::Time& dt)
{
	const float dx = -move_speed * dt.asSeconds();
	ground_sprite1.move({ dx, 0.f });
	ground_sprite2.move({ dx, 0.f });

	const sf::FloatRect g1 = ground_sprite1.getGlobalBounds();
	const sf::FloatRect g2 = ground_sprite2.getGlobalBounds();

	if (g1.position.x + g1.size.x < 0.f)
		ground_sprite1.setPosition({ g2.position.x + g2.size.x, GROUND_Y });
	if (g2.position.x + g2.size.x < 0.f)
		ground_sprite2.setPosition({ g1.position.x + g1.size.x, GROUND_Y });
}

void Game::restartGame()
{
	bird.resetBirdPosition();
	bird.setShouldFly(false);
	run_game = true;
	is_enter_pressed = false;
	spawn_timer = 0.f;
	pipes.clear();
	score = 0;
	score_text.setString("Score: 0");
}

void Game::draw()
{
	win.clear();
	win.draw(bg_sprite);
	for (const Pipe& pipe : pipes)
	{
		win.draw(pipe.sprite_down);
		win.draw(pipe.sprite_up);
	}
	win.draw(ground_sprite1);
	win.draw(ground_sprite2);
	win.draw(bird.bird_sprite);
	win.draw(score_text);

	if (run_game && !is_enter_pressed)
		win.draw(hint_text);

	if (!run_game)
	{
		win.draw(game_over_text);
		win.draw(restart_text);
	}
}