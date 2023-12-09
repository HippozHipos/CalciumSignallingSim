#include "Cycpch.h"
#include "Cyclopes/Core/ResourceManager.h"
#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLTexture.h"

namespace cyc {

	CycUnorderedMap<CycString, CycShared<Texture2D>> TextureManager::m_2DTextures;
	CycUnorderedMap<CycString, int> TextureManager::m_2DTextureIDs;

	void TextureManager::_Init()
	{

#if defined(CYC_PLATFORM_WINDOWS)
		CycShared<Texture2D> texture = Texture2D::Create(Texture2D::Type::OPENGL);
#endif
		uint8_t whiteTexture[] = { 255, 255, 255, 255 };
		texture->LoadRawData(whiteTexture, sizeof(uint8_t) * 4);
		texture->BufferData(1, 1);
		const char* name = "WhiteTexture1x1";
		m_2DTextures.insert(std::make_pair(name, texture));
		m_2DTextureIDs.insert(std::make_pair(name, 0));
	}

	Texture2D* TextureManager::Create2D(const char* name, const char* path)
	{
		int slot = (int)m_2DTextureIDs.size();
#if defined(CYC_PLATFORM_WINDOWS)
		CycShared<Texture2D> texture = Texture2D::Create(Texture2D::Type::OPENGL, path, slot);
#endif
		m_2DTextures.insert(std::make_pair(name, texture));
		m_2DTextureIDs.insert(std::make_pair(name, slot));
		return texture.get();
	}

	int TextureManager::GetID2D(const char* name)
	{
		return m_2DTextureIDs.find(name)->second;
	}

	size_t TextureManager::Count2D() 
	{
		return m_2DTextures.size();
	}

	Texture2D* TextureManager::Get2D(const char* name)
	{
		return m_2DTextures.find(name)->second.get();
	}

}