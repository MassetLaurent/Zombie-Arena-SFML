#pragma once

#include <SFML/Graphics.hpp>

class Zombie
{
private:
	// How fast is each zombie types
	const float CHASER_SPEED	{ 80.0f };
	const float BLOATER_SPEED	{ 40.0f };
	const float CRAWLER_SPEED	{ 20.0f };

	// How much health does each zombie type have
	const float BLOATER_HEALTH	{ 5.0f };
	const float CRAWLER_HEALTH	{ 3.0f };
	const float CHASER_HEALTH	{ 1.0f };

	// Let every type of zombie vary its speed slightly
	const int MAX_VARIANCE	{ 30 };
	const int OFFSET		{ 101 - MAX_VARIANCE };

	// Where is the zombie in the world
	sf::Vector2f m_position;

	//Sprite for the zombie
	sf::Sprite m_sprite;
	sf::RectangleShape m_life;

	// How fast is the zombie
	float m_speed;

	// How much health does the zombie have
	float m_health;

	// Is the zombie alive or not
	bool m_alive;

public:
	Zombie();
	~Zombie();

	// Handle when the zombie is getting hit by a bullet
	bool hit();
	
	//Spawn a new zombie and set its type (1, 2, or 3)
	void spawn(float startX, float startY, int type, int seed);

	
	// Getters for the zombie's data members
	sf::FloatRect		getPosition() const		{ return m_sprite.getGlobalBounds(); }
	sf::Sprite			getSprite() const { return m_sprite; } // Get the sprite of the zombie, which will be used to draw it on the screen
	sf::RectangleShape	getLifeShape() const { return m_life; } // Get the sprite of the zombie, which will be used to draw it on the screen
	sf::Vector2f		getCenter() const		{ return m_position; }
	float				getRotation() const		{ return m_sprite.getRotation(); }
	bool				isAlive() const			{ return m_alive; }

	// Update the zombie's each frame, making it move towards the player and rotate to face the player
	void update(float deltaTime, sf::Vector2f playerPosition);

};

