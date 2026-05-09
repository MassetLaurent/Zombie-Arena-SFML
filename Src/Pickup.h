#pragma once
#include <SFML/Graphics.hpp>

class Pickup
{
private:
	const int HEALTH_START_VALUE	{ 50 };
	const int AMMO_START_VALUE		{ 12 };
	const int START_WAIT_TIME		{ 10 };
	const int START_SECOND_TO_LIVE	{ 5 };

	int m_value						{ 0 };
	int m_type						{ 0 };
	
	float m_secondsSinceSpawn		{ 0.f };
	float m_secondsSinceDespawn		{ 0.f };
	float m_secondsToLive			{ 0.f };
	float m_secondsToWait			{ 0.f };

	bool m_spawned					{ false };
	
	sf::Sprite m_sprite;

	sf::IntRect m_Arena;

public:
	Pickup(int type);
	~Pickup();

	void setArena(sf::IntRect arena);

	void spawn();
	void despawn();

	sf::FloatRect getPosition() const { return m_sprite.getGlobalBounds(); }
	sf::Sprite		getSprite() const { return m_sprite; }
	bool			isSpawned() const { return m_spawned; }

	void update(float deltaTime);


	int gotIt();
	
	void upgrade();
};