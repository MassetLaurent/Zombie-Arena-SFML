#include <iostream>

#include "Player.h"
#include "TextureHolder.h"

Player::Player() : m_speed(START_SPEED), m_health(START_HEALTH), m_maxHealth(START_HEALTH)
{

	if (!m_texture.loadFromFile("Res/Textures/player.png"))
		std::cerr << "Error loading player texture" << std::endl;

	m_sprite.setTexture(m_texture);

	m_sprite.setTexture(TextureHolder::GetTexture("Res/Textures/player.png"));

	m_sprite.setOrigin(25.0f, 25.0f);

	m_position = sf::Vector2f(50.0f, 50.0f);
	m_sprite.setPosition(m_position);

	m_resolution = sf::Vector2f(0.0f, 0.0f);
	m_arena = sf::IntRect(0, 0, 500, 500);
	m_lastHit = sf::Time::Zero;

	m_tileSize = 50;

	m_upPressed = false;
	m_downPressed = false;
	m_leftPressed = false;
	m_rightPressed = false;
}

Player::~Player()
{
}

void Player::Spawn(sf::IntRect arena, sf::Vector2f resolution, int tileSize)
{
	m_arena = arena;

	m_position.x = arena.width / 2.0f;
	m_position.y = arena.height / 2.0f;
	m_sprite.setPosition(m_position);
		
	m_resolution = resolution;
	m_tileSize = tileSize;
}

void Player::ResetStats()
{
	m_speed = START_SPEED;
	m_health = START_HEALTH;
	m_maxHealth = START_HEALTH;
}

bool Player::hit(sf::Time timeHit)
{
	if (timeHit.asMilliseconds() - m_lastHit.asMilliseconds() > 200)
	{
		m_lastHit = timeHit;
		m_health -= 10;
		return true;
	}
	else
		return false;
}

void Player::update(float elapsedTime, sf::Vector2i mousePosition)
{
	// Update position based on input
	if (m_upPressed)
		m_position.y -= m_speed * elapsedTime;
	if (m_downPressed)
		m_position.y += m_speed * elapsedTime;
	if (m_leftPressed)
		m_position.x -= m_speed * elapsedTime;
	if (m_rightPressed)
		m_position.x += m_speed * elapsedTime;

	// Apply new position to player sprite
	m_sprite.setPosition(m_position);

	// Check if player is outside of arena and correct
	if (m_position.x > m_arena.width - m_tileSize - m_sprite.getGlobalBounds().width / 4)
		m_position.x = m_arena.width - m_tileSize - m_sprite.getGlobalBounds().width / 4;
	if (m_position.x < m_arena.left + m_tileSize + m_sprite.getGlobalBounds().width / 4)
		m_position.x = m_arena.left + m_tileSize + m_sprite.getGlobalBounds().width / 4;
	if (m_position.y > m_arena.height - m_tileSize - m_sprite.getGlobalBounds().height / 4)
		m_position.y = m_arena.height - m_tileSize - m_sprite.getGlobalBounds().height / 4;
	if (m_position.y < m_arena.top + m_tileSize + m_sprite.getGlobalBounds().height / 4)
		m_position.y = m_arena.top + m_tileSize + m_sprite.getGlobalBounds().height / 4;

	// Calculate the angle in radians between the player and the mouse position
	float angle = atan2(mousePosition.y - (m_resolution.y / 2), mousePosition.x - (m_resolution.x / 2));
	// Convert the angle to degrees and set the player's rotation
	m_sprite.setRotation((angle * 180) / 3.14159);
}

void Player::render(sf::RenderWindow& window)
{
}

