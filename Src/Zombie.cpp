#include <cstdlib>
#include <ctime>

#include "TextureHolder.h"
#include "Zombie.h"

Zombie::Zombie()
{
}

Zombie::~Zombie()
{
}

bool Zombie::hit()
{
	m_health--; 

	if (m_health <= 0)
	{
		m_alive = false;
		m_sprite.setTexture(TextureHolder::GetTexture("Res/Textures/blood.png"));

		return true;
	}

	// Injured but not dead yet
	return false;
}

void Zombie::spawn(float startX, float startY, int type, int seed)
{
	// Adjust health & speed based on the type of zombie
	switch (type)
	{
	case 0:
		// Bloater zombie type
		// Request the texture for the bloater zombie from the TextureHolder and set it to the sprite of the zombie
		m_sprite.setTexture(TextureHolder::GetTexture("Res/Textures/bloater.png"));
		m_speed = BLOATER_SPEED;
		m_health = BLOATER_HEALTH;
		break;
	case 1:
		// Chaser zombie type
		// Request the texture for the chaser zombie from the TextureHolder and set it to the sprite of the zombie
		m_sprite.setTexture(TextureHolder::GetTexture("Res/Textures/chaser.png"));
		m_speed = CHASER_SPEED;
		m_health = CHASER_HEALTH;
		break;
	case 2:
		// Crawler zombie type
		// Request the texture for the crawler zombie from the TextureHolder and set it to the sprite of the zombie
		m_sprite.setTexture(TextureHolder::GetTexture("Res/Textures/crawler.png"));
		m_speed = CRAWLER_SPEED;
		m_health = CRAWLER_HEALTH;
		break;
	}

	// Apply a random variance to the speed of the zombie to make them less predictable
	srand((int)time(0) * (seed)); 

	// Generate a random number in between OFFSET and OFFSET + MAX_VARIANCE to use as a modifier for the zombie's speed
	float modifier = (rand() % MAX_VARIANCE) + OFFSET;
	modifier /= 100; 			// Convert the modifier to a percentage by dividing it by 100
	m_speed *= modifier;		// Apply the modifier to the zombie's speed

	// Set the position of the zombie
	m_position.x = startX;
	m_position.y = startY;

	// Set the sprite of the zombie based on its type
	m_sprite.setOrigin(25.0f, 25.0f);
	m_sprite.setPosition(m_position);

}

void Zombie::update(float deltaTime, sf::Vector2f playerPosition)
{
	// Update the zombie's position to move towards the player
	if (playerPosition.x > m_position.x)
		m_position.x += m_speed * deltaTime;
	else
		m_position.x -= m_speed * deltaTime;

	if (playerPosition.y > m_position.y)
		m_position.y += m_speed * deltaTime;
	else
		m_position.y -= m_speed * deltaTime;

	// Update the sprite's position to match the zombie's position
	m_sprite.setPosition(m_position);

	// Face the zombie towards the player by calculating the angle between the zombie and the player and setting the sprite's rotation to that angle
	float angle = atan2(playerPosition.y - m_position.y, playerPosition.x - m_position.x) * 180 / 3.14159f; // Calculate the angle in degrees using atan2
	m_sprite.setRotation(angle); // Apply the angle to the sprite's rotation
}
