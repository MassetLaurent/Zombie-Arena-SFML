#include "Pickup.h"
#include "TextureHolder.h"

Pickup::Pickup(int type) :m_type(type)
{
	switch (type)
	{
		case 1:
			m_sprite.setTexture(TextureHolder::GetTexture("Res/Textures/health_pickup.png"));
			m_value = HEALTH_START_VALUE;
			break;
		default:
			m_sprite.setTexture(TextureHolder::GetTexture("Res/Textures/ammo_pickup.png"));
			m_value = AMMO_START_VALUE;
			break;
	}

	//m_sprite.setOrigin(25, 25);
	m_secondsToLive = START_SECOND_TO_LIVE;
	m_secondsToWait = START_WAIT_TIME;
}

Pickup::~Pickup()
{

}

void Pickup::setArena(sf::IntRect arena)
{
	// Reduce the size of the arena by 50 pixel on each border
	m_Arena.left = arena.left + 50;
	m_Arena.width = arena.width - 50;
	m_Arena.top = arena.top + 50;
	m_Arena.height = arena.height - 50;

	this->spawn();
}

void Pickup::spawn()
{
	// Spawn pickup item on random location within the arena size
	srand((int)time(0) / m_type);
	int x{ rand() % m_Arena.width - 100};

	if (x < 50)
		x = 50;

	srand((int)time(0) * m_type);
	int y{ rand() % m_Arena.height - 100};

	if (y < 50)
		y = 50;

	m_secondsSinceSpawn = 0;
	m_spawned = true;

	m_sprite.setPosition(x, y);
}

void Pickup::despawn()
{
	m_spawned = false();
	m_secondsSinceDespawn = 0;
	m_secondsToWait = 10.f;
}

void Pickup::update(float deltaTime)
{
	if (m_spawned)
		m_secondsSinceSpawn += deltaTime;
	else
		m_secondsSinceDespawn += deltaTime;

	if (m_secondsSinceSpawn > m_secondsToLive && m_spawned)
		this->despawn();

	if (m_secondsSinceDespawn > m_secondsToWait && !m_spawned)
		this->spawn();
}

int Pickup::gotIt()
{
	this->despawn();
	return m_value;
}

void Pickup::upgrade()
{
	if (m_type == 1)
		m_value += (HEALTH_START_VALUE * 0.5);
	else
		m_value += (AMMO_START_VALUE * 0.5);

	m_secondsToLive += (START_SECOND_TO_LIVE / 10);
	m_secondsToWait - +(START_WAIT_TIME / 10);
}

