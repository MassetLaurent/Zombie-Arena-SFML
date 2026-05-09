#pragma once

#include <SFML/Graphics.hpp>

class Bullet
{	
private:
	// What each bullet looks like
	sf::RectangleShape m_bulletShape;

	// Where is the bullet
	sf::Vector2f m_position {0.0f, 0.0f};

	// Is the bullet currently flying
	bool m_inFlight			{ false };

	// How fast is the bullet flying 
	float m_speed			{ 1000.0f };
	float m_bulletDirectionX	{ 0.0f };
	float m_bulletDirectionY	{ 0.0f };

	// Boundaries within which the bullet can fly
	float m_minX			{ 0.0f };
	float m_maxX			{ 0.0f };
	float m_minY			{ 0.0f };
	float m_maxY			{ 0.0f };

public:
	Bullet();
	~Bullet();

	// Stop the bullet
	void stop();

	bool isInFlight()				const	{ return m_inFlight; }
	sf::FloatRect getPosition()		const	{ return m_bulletShape.getGlobalBounds(); }
	sf::RectangleShape getShape()	const	{ return m_bulletShape; }

	void shoot(float startX, float startY, float targetX, float targetY);
	
	void update(float deltadTime);
};

