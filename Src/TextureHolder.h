#pragma once

#include <map>
#include <SFML/Graphics.hpp>

class TextureHolder
{
private:
	// Create a map of textures so we can associate a name with each texture
	std::map<std::string, sf::Texture> m_Textures;

	// Pointer to the single instance of TextureHolder itself
	static TextureHolder* m_s_Instance;

public:
	TextureHolder();
	static sf::Texture& GetTexture(std::string const& filename);
};

