#include "ZombieArena.h"

int createBackgound(sf::VertexArray& rVA, sf::IntRect arena)
{
	const int TILE_SIZE			{ 50 };		// The size of each tile in pixels
	const int TILE_TYPES		{ 3 };		// The number of different tile types (e.g., floor, wall, etc.)
	const int VERTS_IN_QUAD		{ 4 };		// The number of vertices in a quad
	int currentVertex			{ 0 };		// A counter to keep track of the current vertex index in the vertex array

	int worldWidth  {arena.width / TILE_SIZE};		// Define the number of tiles in the x direction
	int worldHeight {arena.height / TILE_SIZE};		// Define the number of tiles in the y direction
	
	rVA.setPrimitiveType(sf::Quads);						// Set the primitive type to quads, which means that every 4 vertices will form a quad (a rectangle)
	rVA.resize(worldWidth * worldHeight * VERTS_IN_QUAD);	// Resize the vertex array to fit all the tiles in the world (worldWidth * worldHeight) and each tile is made of 4 vertices (VERTS_IN_QUAD)
		
	for (size_t w{ 0 }; w < worldWidth; ++w)	// Loop through each tile in the x direction
	{
		for(size_t h{0}; h < worldHeight; ++h)	// Loop through each tile in the y direction
		{
			rVA[currentVertex + 0].position = sf::Vector2f (w * TILE_SIZE, h * TILE_SIZE);				// Top-left vertex of the quad
			rVA[currentVertex + 1].position = sf::Vector2f((w + 1) * TILE_SIZE, h * TILE_SIZE);			// Top-right vertex of the quad
			rVA[currentVertex + 2].position = sf::Vector2f((w + 1) * TILE_SIZE, (h + 1) * TILE_SIZE);	// Bottom-right vertex of the quad
			rVA[currentVertex + 3].position = sf::Vector2f (w * TILE_SIZE, (h + 1) * TILE_SIZE);		// Bottom-left vertex of the quad

			// If the tile is on the edge of the world, make it a wall tile
			if (h == 0 || h == worldHeight - 1 || w == 0 || w == worldWidth - 1)						
			{
				rVA[currentVertex + 0].texCoords = sf::Vector2f(0, 0 + TILE_TYPES * TILE_SIZE);			
				rVA[currentVertex + 1].texCoords = sf::Vector2f(TILE_SIZE, 0 + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 2].texCoords = sf::Vector2f(TILE_SIZE, TILE_SIZE + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 3].texCoords = sf::Vector2f(0, TILE_SIZE + TILE_TYPES * TILE_SIZE);
			}
			// If the tile is not on the edge, randomly select a tile type for it
			else
			{
				// Init the seed for the random number generator with a combination of the current time and the tile's position to ensure different random values for each tile and each run of the program
				srand((int)time(0) + h * w - w); 

				// Randomly decide if the tile is mud or grass by generating a random number in between the number of tiles types
				int mudOrGrass {(rand() % TILE_TYPES)}; 

				// Randomly select a tile type for this quad (e.g., floor, wall, etc.) by generating a random number between 0 and TILE_TYPES - 1
				int verticalOffset {mudOrGrass * TILE_SIZE};

				rVA[currentVertex + 0].texCoords = sf::Vector2f(0, 0 + verticalOffset);					// Texture coordinate for the top-left vertex of the quad
				rVA[currentVertex + 1].texCoords = sf::Vector2f(TILE_SIZE, 0 + verticalOffset);			// Texture coordinate for the top-right vertex of the quad
				rVA[currentVertex + 2].texCoords = sf::Vector2f(TILE_SIZE, TILE_SIZE + verticalOffset);	// Texture coordinate for the bottom-right vertex of the quad
				rVA[currentVertex + 3].texCoords = sf::Vector2f(0, TILE_SIZE + verticalOffset);			// Texture coordinate for the bottom-left vertex of the quad
			}
			
			currentVertex += VERTS_IN_QUAD; // Move to the next set of vertices for the next quad
		}
	}
	return TILE_SIZE;
}