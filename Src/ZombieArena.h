#pragma once
#include <SFML/Graphics.hpp>

#include "Zombie.h"

int createBackgound(sf::VertexArray& rVA, sf::IntRect arena);
Zombie* createHorde(int numZombies, sf::IntRect arena);
