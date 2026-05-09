#include "Bullet.h"

Bullet::Bullet()
{
	m_bulletShape.setSize(sf::Vector2f(25.0f, 5.0f));
	m_bulletShape.setFillColor(sf::Color::Red);
}

Bullet::~Bullet()
{
}

void Bullet::stop()
{
	m_inFlight = false;
}

void Bullet::shoot(float startX, float startY, float targetX, float targetY)
{
	m_inFlight = true;
	m_position.x = startX;
	m_position.y = startY;

	// Calculate the gradiant of the line between the start and target points
	float gradiant{ (startX - targetX) / (startY - targetY) };

	if(gradiant < 0)
		gradiant *= -1;
	
	float ratioXY{ m_speed / (1 + gradiant) };

	m_bulletDirectionX = ratioXY * gradiant;
	m_bulletDirectionY = ratioXY;

	if (targetX < startX)
		m_bulletDirectionX *= -1;
	if (targetY < startY)
		m_bulletDirectionY *= -1;

	float angle = atan2(targetY - startY, targetX - startX );
	// Convert the angle to degrees and set the player's rotation
	m_bulletShape.setRotation((angle * 180) / 3.14159);

	// Set the boundaries for the bullet to fly in the range
	float range{ 500.0f };
	m_minX = startX - range;
	m_maxX = startX + range;
	m_minY = startY - range;
	m_maxY = startY + range;

	m_bulletShape.setPosition(m_position);
}

void Bullet::update(float deltadTime)
{
	// Set the new position of the bullet based on the time passed since the last update
	m_position.x += m_bulletDirectionX * deltadTime;
	m_position.y += m_bulletDirectionY * deltadTime;

	m_bulletShape.setPosition(m_position);

	// Has the bullet gone out of bounds?
	if(m_position.x < m_minX || m_position.x > m_maxX || m_position.y < m_minY || m_position.y > m_maxY)
		this->stop();


}
