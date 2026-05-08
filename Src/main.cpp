#include <SFML/Graphics.hpp>
#include <iostream>

#include "Player.h"
#include "TextureHolder.h"
#include "ZombieArena.h"

int main()
{
	// =========================== =========================//
	//														// 
	//					Game window & Views	 				//
	//														//
	// =========================== =========================//
	// Get the desktop resolution and create the game window with that resolution
	sf::Vector2f resolution{ 0.f, 0.f };
	resolution.x = sf::VideoMode::getDesktopMode().width;
	resolution.y = sf::VideoMode::getDesktopMode().height;
	sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Zombie Arena", sf::Style::Default);
	window.setPosition(sf::Vector2i(-9, 0)); // Repositioning window by 9 pixels to compensate for the Windows desktop border
	sf::Event event; // Event object to hold the events polled from the window

	// Create the main view with the same size as the resolution of the window
	sf::View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Vectors to hold the mouse position in world coordinates and screen coordinates, and the player position in world coordinates
	sf::Vector2f mouseWorldPosition;
	sf::Vector2i mouseScreenPosition;


		// Places all controlers befor using them in order to avoid undefined behavior
		// Textures manager, sound manager, entities manager and font manager are all created before the game loop to load and hold all the resources for the game
		// and to avoid loading them multiple times during the game loop which would cause performance issues and memory leaks
		

	// =========================== =========================//
	//														// 
	//						  Textures						//
	//														//
	// =========================== =========================//
	TextureHolder textureHolder; // Create the texture holder object, which will be used to load and hold all the textures for the game
	
	sf::Texture textureBackground{ textureHolder.GetTexture("Res/Textures/background_sheet.png") };

	// Create the background vertex array, which will be used to draw the background of the arena
	sf::VertexArray backgroundVA;


	// =========================== =========================//
	//														// 
	//							Audio						// 
	//														//
	// =========================== =========================//




	// =========================== =========================//
	//														// 
	//							Entities					//
	//														//
	// =========================== =========================//
		// Player use TextureHolder to load and hold the texture for the player,
		// so we need to create the TextureHolder before creating the player
		
	// Create the player object
	Player player; 

	sf::Vector2f playerPosition;

	// Create the horde of zombies, which will be updated later based on the level up screen choices
	int numZombies{ 10 };
	int numZombiesAlive{ numZombies };
	Zombie* zombies = nullptr;


	// =========================== =========================//
	//														// 
	//							Texts						//
	//														//
	// =========================== =========================//


	// =========================== =========================//
	//														// 
	//							Time						//
	//														//
	// =========================== =========================//
	// // Time related variables to control the time it takes to update and render each frame, and to keep track of the total game time
	sf::Clock clock;
	sf::Time gameTimeTotal;
	sf::Time deltaTime;
	float dtAsSeconds;


	// =========================== =========================//
	//														// 
	//						Game Logic						//
	//														//
	// =========================== =========================//
	enum class GameState { PAUSED, LEVELING_UP, GAME_OVER, PLAYING }; // states of the game, used to control the flow of the game and what actions are allowed in each state
	GameState state = GameState::LEVELING_UP; // first state of the game
		
	// Initial size of the arena, which will be updated later based on the level up screen choices
	sf::IntRect arena{ 0,0,500,300 };
	

	// =========================== =========================//
	//														// 
	//						  Game Loop						//
	//														//
	// =========================== =========================//
	while (window.isOpen())
	{
		// ====================================================== Window Poll Events ====================================================//
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



		// ====================================================== Handle INPUT ====================================================//
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
				
				// Create the horde of zombies
				delete[] zombies; // Delete the previous horde of zombies (if exist) to prevent memory leaks
				zombies = createHorde(numZombies, arena);

				// Reset the clock so there isn't a jump in time when resuming the game
				clock.restart(); 
			}
		}



		// ====================================================== Update game ====================================================//
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

			// Update the zombies, but only if we are in the playing state
			for (size_t i { 0 }; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
					zombies[i].update(dtAsSeconds, playerPosition);
			}
		}



		// ====================================================== Render & Draw ====================================================//
		if (state == GameState::PLAYING)
		{
			window.clear(sf::Color(16, 16, 16, 255));
			window.setView(mainView);
			window.draw(backgroundVA, &textureBackground);
			window.draw(player.getSprite());

			// Draw the zombies, but only if we are in the playing state
			for (size_t i { 0 }; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
					window.draw(zombies[i].getSprite());
			}
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

	// ====================================================== Clean memory & exit ====================================================//
	delete[] zombies; // Delete the horde of zombies to prevent memory leaks

	return 0;
}