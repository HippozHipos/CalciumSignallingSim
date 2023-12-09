#include "Cycpch.h"
#include "Texture.h"
#include "Cyclopes/Core/Assert.h"
#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLTexture.h"

#include <stbimg/stbimg.h>

namespace cyc {

	Texture2D::Texture2D()
	{
	}

	Texture2D::Texture2D(const char* path)
	{
		LoadFromFile(path);
	}

	Texture2D::Texture2D(const Texture2D& other)
	{
		LoadRawData(other.GetRawData(), other.Size());
	}

	Texture2D& Texture2D::operator=(const Texture2D& other)
	{
		if (this == &other)
			return *this;
		LoadRawData(other.GetRawData(), other.Size());
		return *this;
	}

	Texture2D::~Texture2D()
	{
		if (IsLoaded())
			DeleteData();
	}

	CycShared<Texture2D> Texture2D::Create(Type type, uint8_t slot)
	{
		switch (type)
		{
			case Type::OPENGL: return CycMakeShared<OpenGLTexture2D>(slot);
		}
		CYC_ASSERT(false, "Texture2D of given type could not be created");
	}

	Texture2D* Texture2D::CreateOwning(Type type, uint8_t slot)
	{
		switch (type)
		{
		case Type::OPENGL: return new OpenGLTexture2D(slot);
		}
		CYC_ASSERT(false, "Texture2D of given type could not be created");
	}


	CycShared<Texture2D> Texture2D::Create(Type type, const char* path, uint8_t slot)
	{
		switch (type)
		{
			case Type::OPENGL: return CycMakeShared<OpenGLTexture2D>(path, slot);
		}
		CYC_ASSERT(false, "Texture2D of given type could not be created");
	}

	Texture2D* Texture2D::CreateOwning(Type type, const char* path, uint8_t slot)
	{
		switch (type)
		{
			case Type::OPENGL: return new OpenGLTexture2D(path, slot);
		}
		CYC_ASSERT(false, "Texture2D of given type could not be created");
	}

	void Texture2D::LoadFromFile(const char* path)
	{
		if (IsLoaded())
			DeleteData();
		stbi_set_flip_vertically_on_load(m_FlipOnLoad);
		m_Data = stbi_load(path, &m_Width, &m_Height, &m_NChannels, 0);
		CYC_ASSERT_NOBREAK(m_Data, "Could not load texture from file");
		m_IsLoaded = true;
	}

	void Texture2D::LoadRawData(void* data, std::size_t size)
	{
		if (IsLoaded())
			DeleteData();
		m_Data = (std::uint8_t*)malloc(size);
		CYC_ASSERT(m_Data, "Could not allocate memory for texture");
		memcpy(m_Data, data, size);
		m_IsLoaded = true;
	}

	void Texture2D::FlipOnLoad(bool flip) 
	{
		m_FlipOnLoad = flip;
	}

	std::uint8_t* Texture2D::GetRawData()
	{
		return m_Data;
	}

	uint8_t* Texture2D::GetRawData() const
	{
		return m_Data;
	}

	bool Texture2D::IsLoaded() const
	{
		return m_IsLoaded;
	}

	void Texture2D::DeleteData()
	{
		free(m_Data);
		m_Width = m_Height = m_NChannels = 0;
		m_IsLoaded = false;
	}
	void Texture2D::SetMitMapLevel(int level)
	{
		m_MitMapLevel = level;
	}

	int Texture2D::GetMitMapLevel() const
	{
		return m_MitMapLevel;
	}

	int Texture2D::Size() const
	{
		return m_Width * m_Height * m_NChannels;
	}
}