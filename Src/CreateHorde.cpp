#include "Zombie.h"
#include "ZombieArena.h"

Zombie* createHorde(int numZombies, sf::IntRect arena)
{
	const int OFFSET { 25 }; // The distance from the edge of the arena that zombies will spawn at to prevent them from spawning partially outside the arena

	int min_X{ arena.left + OFFSET };
	int max_X{ arena.width - OFFSET };

	int min_Y{ arena.top + OFFSET };
	int max_Y{ arena.height - OFFSET };

	float x { 0 }, y { 0 }; // Future variables to hold the x and y coordinates of the zombie spawn location
	
	// Create a dynamic array of zombies to hold the horde of zombies that we will create
	// We will return a pointer to this array, so we need to create it on the heap using NEW - think to delete this array later to prevent memory leaks
	Zombie* zombies = new Zombie[numZombies];

	// Loop through the number of zombies and spawn each zombie at a random location on the edge of the arena with a random type
	for (size_t i{ 0 }; i < numZombies; ++i)
	{
		// Spawn the zombie at a random location on the edge of the arena
		srand((int)time(0) * (i + 1)); // Init the seed for the random number generator with a combination of the current time and the index of the zombie to ensure different random values for each zombie and each run of the program
		
		// Randomly select a side of the arena (0: top, 1: right, 2: bottom, 3: left)
		int side {(rand() % 4)}; 


		switch (side)
		{
		case 0: // Top
			x = (rand() % max_X) + min_X;
			y = min_Y;
			break;
		case 1: // Right
			x = max_X;
			y = (rand() % max_Y) + min_Y;
			break;
		case 2: // Bottom
			x = (rand() % max_X) + min_X;
			y = max_Y;
			break;
		case 3: // Left
			x = min_X;
			y = (rand() % max_Y) + min_Y;
			break;
		}

		// Re-init the seed for the random number generator with a different combination of the current time and the index of the zombie to ensure different random values for each zombie and each run of the program
		srand((int)time(0)* i * 2);
		// Randomly select a type for the zombie (0, 1, or 2)
		int type {(rand() % 3)}; 
		
		// Spawn the zombie with its position, type, and index as seed for random variance in speed
		zombies[i].spawn(x, y, type, i); 
	}

	return zombies;
}