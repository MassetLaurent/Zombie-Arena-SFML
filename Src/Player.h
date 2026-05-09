#pragma once
#include <SFML/Graphics.hpp>

class Player
{
private:
	const float		START_SPEED = 200.0f;
	const float		START_HEALTH = 100;

	sf::Texture			m_texture;
	sf::Sprite			m_sprite;
	sf::Vector2f		m_position		{ 0.f,0.f };
	sf::Vector2f		m_resolution	{ 0.f,0.f };
	sf::IntRect			m_arena;
	sf::RectangleShape	m_healthBar;
	sf::Time			m_lastHit;

	int				m_tileSize;

	float			m_health;
	float			m_maxHealth;
	float			m_speed;

	bool			m_upPressed;
	bool			m_downPressed;
	bool			m_leftPressed;
	bool			m_rightPressed;

public:
	Player();
	~Player();

	void Spawn(sf::IntRect arena, sf::Vector2f resolution, int tileSize);

	void ResetStats();

	bool hit(sf::Time timeHit);

	sf::Time getLastHitTime() const			{ return m_lastHit; }
	sf::FloatRect getPosition() const		{ return m_sprite.getGlobalBounds(); }
	sf::Vector2f getCenter() const			{ return m_position; }
	sf::Sprite getSprite() const			{ return m_sprite; }
	sf::RectangleShape getHealthBar() const { return m_healthBar; }

	float getRotation() const { return m_sprite.getRotation(); }

	float getHealth() const { return m_health; }

	void moveLeft() { m_leftPressed = true; }
	void moveRight() { m_rightPressed = true; }
	void moveUp() { m_upPressed = true; }
	void moveDown() { m_downPressed = true; }
	void stopLeft() { m_leftPressed = false; }
	void stopRight() { m_rightPressed = false; }
	void stopUp() { m_upPressed = false; }
	void stopDown() { m_downPressed = false; }

	void upgradeSpeed() { m_speed += START_SPEED * .2f; }
	void upgradeHealth() { m_maxHealth += START_HEALTH * .2f; }
	void heal(int amount) { m_health += amount; if (m_health > m_maxHealth) m_health = m_maxHealth; }

	void update(float elapsedTime, sf::Vector2i mousePosition);
	void render(sf::RenderWindow& window);
};

