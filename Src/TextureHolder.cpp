#include <iostream>
#include "TextureHolder.h"

// Include assert to check if the texture loading succeeded
#include <assert.h>

TextureHolder* TextureHolder::m_s_Instance {nullptr};

TextureHolder::TextureHolder()
{
	// Check if an instance of TextureHolder already exists
	assert(m_s_Instance == nullptr);
	// If not, set the static pointer to this instance
	m_s_Instance = this;
}

sf::Texture& TextureHolder::GetTexture(std::string const& filename)
{
	// Get a reference to the static instance of TextureHolder
	std::map<std::string, sf::Texture>& texturesMap {m_s_Instance->m_Textures};

	auto keyValuePair {texturesMap.find(filename)};

	if (keyValuePair != texturesMap.end())
	{
		// If the texture is found, return it
		return keyValuePair->second;
	}
	else
	{
		// If the texture is not found, load it
		sf::Texture& texture = texturesMap[filename];
		if (!texture.loadFromFile(filename))
		{
			// If loading fails, assert
			std::cerr << "TextureHolder::GetTexture - Failed to load " << filename << std::endl;
			assert(false);
		}
		return texture;
	}
}
