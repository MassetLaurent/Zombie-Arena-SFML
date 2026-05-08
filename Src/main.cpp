#include <SFML/Graphics.hpp>
#include <iostream>

#include "Player.h"
#include "ZombieArena.h"

int main()
{
	// =========================== =========================//
	//														// 
	//						GAME LOGIC						//
	//														//
	// =========================== =========================//
	enum class GameState { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };
	GameState state = GameState::LEVELING_UP;

	std::vector<int> mesNombres;

	sf::Vector2f resolution {0.f, 0.f};
	resolution.x = sf::VideoMode::getDesktopMode().width;
	resolution.y = sf::VideoMode::getDesktopMode().height;

	sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Zombie Arena", sf::Style::Default);
	window.setPosition(sf::Vector2i(-9, 0)); // décalage de 9 pixels pour compenser la bordure du bureau windows
	sf::Event event;

	sf::View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	sf::Clock clock;
	sf::Time gameTimeTotal;
	sf::Time deltaTime;
	float dtAsSeconds;

	sf::Vector2f mouseWorldPosition;
	sf::Vector2i mouseScreenPosition;
	sf::Vector2f playerPosition;

	Player player;

	sf::VertexArray backgroundVA;

	// Initial size of the arena, which will be updated later based on the level up screen choices
	sf::IntRect arena{ 0,0,500,300 };

	// =========================== =========================//
	//														// 
	//						Textures						//
	//														//
	// =========================== =========================//
	sf::Texture textureBackground;
	if(!textureBackground.loadFromFile("Res/Textures/background_sheet.png"))
		std::cerr << "Error loading background texture in main.cpp" << std::endl;

	// =========================== =========================//
	//														// 
	//							Audio						//
	//														//
	// =========================== =========================//
	

	// =========================== =========================//
	//														// 
	//							Texts						//
	//														//
	// =========================== =========================//
	

	// =========================== =========================//
	//														// 
	//						  GAME LOOP						//
	//														//
	// =========================== =========================//
	while (window.isOpen())
	{
		// =========================== Window Poll Events =========================//
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Return && state == GameState::PLAYING)
				{
					state = GameState::PAUSED;
				}
				else if(event.key.code == sf::Keyboard::Return && state == GameState::PAUSED)
				{
					state = GameState::PLAYING;
					clock.restart(); // Reset the clock so there isn't a jump in time when resuming the game
				}
				else if (event.key.code == sf::Keyboard::Return && state == GameState::GAME_OVER)
				{
					state = GameState::LEVELING_UP;
				}

				if(state == GameState::PLAYING)
				{
					
				}
			}
		}

		// =========================== Handle INPUT =========================//
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();

		if (state == GameState::PLAYING) //ZQSD for movement, but only if the game is in the playing state
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
				player.moveUp();
			else
				player.stopUp();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				player.moveDown();
			else
				player.stopDown();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				player.moveLeft();
			else
				player.stopLeft();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				player.moveRight();
			else
				player.stopRight();
		}

		if (state == GameState::LEVELING_UP) // Choose an upgrade, but only if we are in the leveling up state
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
			{
				player.upgradeHealth();
				state = GameState::PLAYING;
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
			{
				player.upgradeSpeed();
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
			{
				player.heal(50);
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4))
			{
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5))
			{
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6))
			{
				state = GameState::PLAYING;
			}

			// after choosing an upgrade, we return to the playing state
			if (state == GameState::PLAYING)
			{
				// Create the background vertex array and get the tile size for the player spawn function
				int tileSize{createBackgound(backgroundVA, arena)};

				// Spawn the player in the middle of the arena with the correct tile size and resolution
				player.Spawn(arena, resolution, tileSize);

				// Reset the clock so there isn't a jump in time when resuming the game
				clock.restart(); 
			}
		}

		// =========================== Update game =========================//
		if (state == GameState::PLAYING)
		{	
			// Update the delta time with the time it took to update and render the last frame
			deltaTime = clock.restart();

			// Update the total game time with the delta time
			gameTimeTotal += deltaTime;

			dtAsSeconds = deltaTime.asSeconds();

			// Update the mouse position relative to the window
			mouseScreenPosition = sf::Mouse::getPosition(window);

			// Convert the mouse position to world coordinates
			mouseWorldPosition = window.mapPixelToCoords(mouseScreenPosition, mainView);

			// Record the player's position
			playerPosition = player.getCenter(); 

			// Update the player, but only if we are in the playing state
			player.update(dtAsSeconds, mouseScreenPosition);

			// Align the main view with the player's position, but only if we are in the playing state
			mainView.setCenter(playerPosition);
		}


		// =========================== RENDER & DRAW=========================//

		if (state == GameState::PLAYING)
		{
			window.clear(sf::Color(16, 16, 16, 255));
			window.setView(mainView);
			window.draw(backgroundVA, &textureBackground);
			window.draw(player.getSprite());
		}
		else if(state == GameState::LEVELING_UP)
		{
			// Draw the leveling up screen
		}
		else if(state == GameState::PAUSED)
		{
			// Draw the paused screen
		}
		else if(state == GameState::GAME_OVER)
		{
			// Draw the game over screen
		}

		window.display();
	}
	//system("PAUSE");

	return 0;
}