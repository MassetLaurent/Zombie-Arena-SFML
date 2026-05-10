#include <fstream>
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>

#include "Bullet.h"
#include "Pickup.h"
#include "Player.h"
#include "TextureHolder.h"
#include "ZombieArena.h"

int main()
{
	// =========================== =========================//
	//														// 
	//							Constants 					//
	//														//
	// =========================== =========================//
	const int HUD_OFFSET{ 50 };
	
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
	window.setMouseCursorVisible(false);
	sf::Event event; // Event object to hold the events polled from the window

	// Create the main view with the same size as the resolution of the window
	sf::View gameView	(sf::FloatRect(0, 0, resolution.x, resolution.y));
	sf::View hudView	(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Vectors to hold the mouse position in world coordinates and screen coordinates, and the player position in world coordinates
	sf::Vector2f mouseWorldPosition;
	sf::Vector2i mouseScreenPosition;

	// =========================== =========================//
	//														// 
	//						Game Logic						//
	//														//
	// =========================== =========================//
	const int KILL_POINTS{ 10 }; // How much score point get granted when kill a zombie
	enum class GameState { PAUSED, LEVELING_UP, GAME_OVER, PLAYING }; // states of the game, used to control the flow of the game and what actions are allowed in each state
	GameState state = GameState::GAME_OVER; // first state of the game

	// Initial size of the arena, which will be updated later based on the level up screen choices
	sf::IntRect arena{ 0,0,1000,700 };

	int score	{ 0 };
	int hiScore	{ 0 };
	int wave	{ 1 };


		// Places all controlers befor using them in order to avoid undefined behavior
		// Textures manager, sound manager, entities manager and font manager are all created before the game loop to load and hold all the resources for the game
		// and to avoid loading them multiple times during the game loop which would cause performance issues and memory leaks
		

	// =========================== =========================//
	//														// 
	//					 Textures & Sprites			 		//
	//														//
	// =========================== =========================//
	TextureHolder textureHolder; // Create the texture holder object, which will be used to load and hold all the textures for the game
	
	sf::Texture textureBackground	{ textureHolder.GetTexture("Res/Textures/background_sheet.png") };
	sf::Texture crosshairTexture	{ textureHolder.GetTexture("Res/Textures/crosshair.png") };

	sf::Sprite crosshairSprite		{ crosshairTexture };
	crosshairSprite.setOrigin(25.f,25.f);
	crosshairSprite.setPosition(10.f,10.f);

	sf::Sprite gameOverSprite;
	gameOverSprite.setTexture(textureHolder.GetTexture("Res/Textures/background.png"));
	gameOverSprite.setPosition(0, 0);
	gameOverSprite.setScale({resolution.x / gameOverSprite.getTextureRect().width, resolution.y / gameOverSprite.getTextureRect().height });

	sf::Sprite ammoIcon;
	ammoIcon.setTexture(textureHolder.GetTexture("Res/Textures/ammo_icon.png"));
	ammoIcon.setPosition(HUD_OFFSET, resolution.y - 150);

	// Create the background vertex array, which will be used to draw the background of the arena
	sf::VertexArray backgroundVA;



	// =========================== =========================//
	//														// 
	//							Audio						// 
	//														//
	// =========================== =========================//
	sf::SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("Res/Audio/hit.wav");
	sf::Sound hit;
	hit.setBuffer(hitBuffer);

	sf::SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("Res/Audio/splat.wav");
	sf::Sound splat;
	splat.setBuffer(splatBuffer);

	sf::SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("Res/Audio/shoot.wav");
	sf::Sound shoot;
	shoot.setBuffer(shootBuffer);

	sf::SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("Res/Audio/reload.wav");
	sf::Sound reload;
	reload.setBuffer(reloadBuffer);

	sf::SoundBuffer reloadFailBuffer;
	reloadFailBuffer.loadFromFile("Res/Audio/reload_failed.wav");
	sf::Sound reloadFail;
	reloadFail.setBuffer(reloadFailBuffer);

	sf::SoundBuffer powerUpBuffer;
	powerUpBuffer.loadFromFile("Res/Audio/reload_failed.wav");
	sf::Sound powerUp;
	powerUp.setBuffer(powerUpBuffer);

	sf::SoundBuffer pickUpBuffer;
	pickUpBuffer.loadFromFile("Res/Audio/pickup.wav");
	sf::Sound pickUp;
	pickUp.setBuffer(pickUpBuffer);
	
	// =========================== =========================//
	//														// 
	//							Time						//
	//														//
	// =========================== =========================//
	// // Time related variables to control the time it takes to update and render each frame, and to keep track of the total game time
	sf::Clock clock;
	sf::Time gameTimeTotal;
	sf::Time deltaTime;
	float dtAsSeconds				{ 0.f };
	float timeSinceLastHudUpdate	{ 0.f };


	
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

	// Ammo and shooting related variables
	const int BULLETS_ARRAY_SIZE{ 50 };	// Maximum number of bullets that can be in flight at the same time, which will be used to control the shooting of the bullets and to prevent memory leaks
	Bullet bullets[BULLETS_ARRAY_SIZE];	// Create an array of bullets, which will be used to shoot the zombies
	int currentBullet	{ 0 };				// Index of the current bullet in the bullets array, which will be used to shoot the zombies
	int bulletsSpare	{ 24 };				// Number of bullets spare to reload (6 magazines of 4 bullets each) (6/24)
	int bulletsInClip	{ 6 };				// Number of bullets currently in the clip, which will be used to control the shooting of the bullets
	int clipSize		{ 6 };					// Number of bullets in each magazine
	float fireRate		{ 0.3f };
	sf::Time lastShot;					// Time when the last shot was fired, which will be used to control the fire rate of the bullets

	//Pickups
	const int NUM_OF_PICKUPS{ 2 };
	Pickup healthPickup(1);
	Pickup ammoPickup(2);



	// =========================== =========================//
	//														// 
	//							Texts						//
	//														//
	// =========================== =========================//
	const int HUD_TEXT_SIZE{ 35 };

	sf::Font font;
	if (!font.loadFromFile("Res/Fonts/font.ttf"))
		std::cerr << "Load font error from main.cpp : Texts section." << std::endl;

	// ====================================================== Game States ======================================================//
	sf::Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(70);
	pausedText.setFillColor(sf::Color::Yellow);
	pausedText.setPosition(resolution.x / 2.5f, resolution.y / 8.f);
	pausedText.setString("		PAUSED ! \n\npress Enter to continue !");

	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(70);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setPosition(resolution.x / 2.5f, resolution.y / 8.f);
	gameOverText.setString("Ppress Enter to continue !");

	sf::Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(50);
	levelUpText.setFillColor(sf::Color::Red);
	levelUpText.setPosition(resolution.x / 2.5f, resolution.y / 8.f);
	std::stringstream levelUpSs;
	levelUpSs << "		UPGRADES !\nchoose with numpad\n\n"
		"1. Increase rate of fire \n" <<
		"2. Increase clip size (next reload) \n" <<
		"3. Increase max health\n" <<
		"4. Increase run speed\n" <<
		"5. More and better health pickups \n" <<
		"6. More and better ammo pickups\n";

	levelUpText.setString(levelUpSs.str());


	// ====================================================== file I/O ======================================================//
	std::ifstream inputFile;
	const std::string filepath{ "gamedata/scores.txt" };

	inputFile.open(filepath);
	if (inputFile.is_open())
	{
		inputFile >> hiScore;
		inputFile.close();
	}
	else
		std::cerr << "Unable to open score file in main.cpp l200." << std::endl;

	// ====================================================== HUD elements ======================================================//
	sf::Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(HUD_TEXT_SIZE);
	ammoText.setFillColor(sf::Color::White);
	std::stringstream ammoTextSs;
	ammoTextSs << bulletsInClip << "/" << bulletsSpare;
	ammoText.setString(ammoTextSs.str());
	ammoText.setPosition(ammoIcon.getPosition().x + 50.f, ammoIcon.getPosition().y + 7.f);

	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(HUD_TEXT_SIZE);
	scoreText.setFillColor(sf::Color::White);
	std::stringstream scoreTextSs;
	scoreTextSs <<  "Score : " << score;
	scoreText.setString(scoreTextSs.str());
	scoreText.setPosition(HUD_OFFSET, 10);

	sf::Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(HUD_TEXT_SIZE);
	hiScoreText.setFillColor(sf::Color::White);
	std::stringstream hiScoreSs;
	hiScoreSs << "High score : " << hiScore;
	hiScoreText.setString(hiScoreSs.str());
	hiScoreText.setPosition(resolution.x - hiScoreText.getLocalBounds().width - HUD_OFFSET, 10);

	sf::Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(HUD_TEXT_SIZE);
	zombiesRemainingText.setFillColor(sf::Color::White);
	std::stringstream zombiesRemainingSs;
	zombiesRemainingSs << "zombies remaining : " << numZombiesAlive;
	zombiesRemainingText.setString(zombiesRemainingSs.str());
	zombiesRemainingText.setPosition(ammoText.getGlobalBounds().left + ammoText.getGlobalBounds().width + HUD_OFFSET, ammoText.getPosition().y);

	sf::Text waveText;
	waveText.setFont(font);
	waveText.setCharacterSize(HUD_TEXT_SIZE);
	waveText.setFillColor(sf::Color::White);
	std::stringstream waveSs;
	waveSs << "Wave : " << wave;
	waveText.setString(waveSs.str());
	waveText.setPosition(resolution.x - waveText.getGlobalBounds().width - HUD_OFFSET, ammoText.getPosition().y);



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
					
					wave = 1;
					score = 0;
					
					currentBullet = 0;
					clipSize = 6;
					bulletsInClip = clipSize;
					bulletsSpare = 24;
					fireRate = 0.8f;
					
					numZombies = 10;
					numZombiesAlive = numZombies;
					
					player.ResetStats();
				}

				if(state == GameState::PLAYING)
				{
					if (event.key.code == sf::Keyboard::R)
					{
						if (bulletsSpare >= clipSize && bulletsInClip < clipSize)
						{
							bulletsSpare -= clipSize-bulletsInClip;
							bulletsInClip = clipSize;
							reload.play();
						}
						else if (bulletsSpare > 0 && bulletsInClip < clipSize)
						{
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
							reload.play();
						}
						else
						{
							// No bullets spare to reload
							reloadFail.play();
							std::cout << "No bullets spare to reload!" << std::endl;
						}
					}
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

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (gameTimeTotal.asSeconds() - lastShot.asSeconds() > fireRate && bulletsInClip > 0)
				{
					// Shoot a bullet
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
					currentBullet++;
					shoot.play();

					if (currentBullet > BULLETS_ARRAY_SIZE - 1)
						currentBullet = 0;

					lastShot = gameTimeTotal;

					bulletsInClip--;
				}
			}
		}

		if (state == GameState::LEVELING_UP) // Choose an upgrade, but only if we are in the leveling up state
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
			{
				fireRate *=0.9f;
				state = GameState::PLAYING;
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
			{
				clipSize += 2;
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
			{
				player.upgradeHealth();
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4))
			{
				player.upgradeSpeed();
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5))
			{
				healthPickup.upgrade();
				state = GameState::PLAYING;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6))
			{
				ammoPickup.upgrade();
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
				numZombiesAlive = numZombies;
				delete[] zombies; // Delete the previous horde of zombies (if exist) to prevent memory leaks
				zombies = createHorde(numZombies, arena);

				// actualise pickups arena
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);

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
			mouseWorldPosition = window.mapPixelToCoords(mouseScreenPosition, gameView);

			// Record the player's position
			playerPosition = player.getCenter(); 

			// Update the player, but only if we are in the playing state
			player.update(dtAsSeconds, mouseScreenPosition);

			// Align the main view with the player's position, but only if we are in the playing state
			gameView.setCenter(playerPosition);

			crosshairSprite.setPosition(mouseWorldPosition);

			// Update the zombies, but only if we are in the playing state
			for (size_t i { 0 }; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
					zombies[i].update(dtAsSeconds, playerPosition);
			}

			// Update each bullet if they are in flight mode
			for (size_t i{ 0 }; i < BULLETS_ARRAY_SIZE; i++)
			{
				if(bullets[i].isInFlight())
					bullets[i].update(dtAsSeconds);
			}

			// update TimeToLive || TimeToWait for pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			// ================ ================= ============ Collision detection for all entities =================== ================== ==========
				// Zombie got shot by a bullet ?
			for (size_t i{ 0 }; i < BULLETS_ARRAY_SIZE; i++)
			{
				for (size_t j{ 0 }; j < numZombies; j++)
				{
					if (bullets[i].isInFlight() && zombies[j].isAlive())
					{
						// Collision check
						if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
						{
							// Stop the bullet to fly
							bullets[i].stop();
							// hit the zombie
							if (zombies[j].hit())
							{
								score += KILL_POINTS;

								if (score > hiScore)
									hiScore = score;

								numZombiesAlive--;

								if (numZombiesAlive == 0)
								{
									wave++;
									numZombies	   += 2 * wave;
									arena.width		= 500 + (100 * wave);
									arena.height	= 300 + (100 * wave);
									powerUp.play(); // Play the sound for powerup
									state = GameState::LEVELING_UP;
								}
							}
						}

					}
				}
			}

			// Collision detection between player & pickups
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				bulletsSpare += ammoPickup.gotIt();
				pickUp.play();
			}

			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.heal(healthPickup.gotIt());
				pickUp.play();
			}

			// Collision detection between player & zombies
			for (size_t i{ 0 }; i < numZombies; i++)
			{
				if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive())
				{
					if (player.hit(gameTimeTotal))
					{
						hit.play();
					}
					if (player.getHealth() <= 0)
					{
						state = GameState::GAME_OVER;
						std::ofstream outputFile("gamedata/scores.txt");
						if (outputFile.is_open())
						{
							outputFile << hiScore;
							outputFile.close();
						}
						else
							std::cerr << "Unable to open gamedata/scores.txt in main.cpp l516" << std::endl;
					}
				}
			}

			// ============= HUD Update ============= //
			timeSinceLastHudUpdate += dtAsSeconds;

			if (timeSinceLastHudUpdate > 0.2f)
			{
				timeSinceLastHudUpdate = 0;

				ammoTextSs.str("");
				ammoTextSs << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ammoTextSs.str());

				zombiesRemainingSs.str("");
				zombiesRemainingSs << "zombies remaining : " << numZombiesAlive;
				zombiesRemainingText.setString(zombiesRemainingSs.str());

				scoreTextSs.str("");
				scoreTextSs << "Score : " << score;
				scoreText.setString(scoreTextSs.str());

				waveSs.str("");
				waveSs << "Wave : " << wave;
				waveText.setString(waveSs.str());

				hiScoreSs.str("");
				hiScoreSs << "High score : " << hiScore;
				hiScoreText.setString(hiScoreSs.str());
			}
		}

		if (state == GameState::LEVELING_UP)
		{
		}

		if (state == GameState::GAME_OVER)
		{
			
		}

		// ====================================================== Render & Draw ====================================================//
		if (state == GameState::PLAYING)
		{
			window.clear(sf::Color(16, 16, 16, 255));
			window.setView(gameView);
			window.draw(backgroundVA, &textureBackground);

			// Draw the zombies, but only if we are in the playing state
			for (size_t i { 0 }; i < numZombies; i++)
			{
				if(!zombies[i].isAlive())
					window.draw(zombies[i].getSprite());
			}
			for (size_t i{ 0 }; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
				{
					window.draw(zombies[i].getLifeShape());
					window.draw(zombies[i].getSprite());
				}
			}

			// Draw all the bullets wich are in flight mode
			for (size_t i{ 0 }; i < BULLETS_ARRAY_SIZE; i++)
			{
				if(bullets[i].isInFlight())
					window.draw(bullets[i].getShape());
			}		

			// Draw pickups if they're spawned
			if (healthPickup.isSpawned())
				window.draw(healthPickup.getSprite());

			if (ammoPickup.isSpawned())
				window.draw(ammoPickup.getSprite());

			window.draw(player.getSprite());
			window.draw(crosshairSprite);


				// ==================== HUD ==================//
			window.setView(hudView);
			window.draw(player.getHealthBar());
			window.draw(ammoIcon);
			window.draw(ammoText);
			window.draw(waveText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(zombiesRemainingText);
			std::cout << "player health : " << player.getHealth() << " / " << player.getHealthMax() << std::endl;

		}
		else if(state == GameState::LEVELING_UP)
		{
			window.clear(sf::Color(16, 16, 60, 55));
			window.setView(hudView);
			gameOverSprite.setColor(sf::Color(255, 255, 255, 50));
			window.draw(gameOverSprite);
			window.draw(levelUpText);
			// Draw the leveling up screen
		}
		else if(state == GameState::PAUSED)
		{
			window.setView(hudView);
			window.draw(pausedText);
			// Draw the paused screen
		}
		else if(state == GameState::GAME_OVER)
		{
			window.clear(sf::Color(60, 16, 16, 55));
			window.setView(hudView);
			gameOverSprite.setColor(sf::Color(255, 255, 255, 255));
			window.draw(gameOverSprite);
			window.draw(gameOverText);
			window.draw(hiScoreText);
			// Draw the game over screen
		}

		window.display();
	}

	// ====================================================== Clean memory & exit ====================================================//
	delete[] zombies; // Delete the horde of zombies to prevent memory leaks

	return 0;
}