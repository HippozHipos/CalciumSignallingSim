#pragma once

#include "stdint.h"
#include "Cyclopes/Core/Base.h"

namespace cyc {

	class Texture2D
	{
		using NoDeleteTextureFunction_t = void(Texture2D*);
	public:
		enum class Type
		{
			OPENGL,
			DX12
		};

	public:
		Texture2D();
		Texture2D(const char* path);
		Texture2D(const Texture2D&);
		Texture2D& operator=(const Texture2D&);
		virtual ~Texture2D();


	public:
		virtual void Init(uint8_t slot = 0) = 0;
		virtual void Bind(uint8_t slot = 0) = 0;
		virtual void UnBind() = 0;
		virtual void BufferData() = 0;
		virtual void BufferData(int width, int height) = 0;

	public:
		static CycShared<Texture2D> Create(Type type, uint8_t slot = 0);
		static Texture2D* CreateOwning(Type type, uint8_t slot = 0);
		static CycShared<Texture2D> Create(Type type, const char* path, uint8_t slot = 0);
		static Texture2D* CreateOwning(Type type, const char* path, uint8_t slot = 0);

		void LoadFromFile(const char* path);
		void LoadRawData(void* data, size_t size);
		void FlipOnLoad(bool flip);
		uint8_t* GetRawData();
		uint8_t* GetRawData() const;
		bool IsLoaded() const;
		void DeleteData();
		void SetMitMapLevel(int level);
		int GetMitMapLevel() const;
		int Size() const;

	protected:
		uint8_t* m_Data = nullptr;
		int m_Width = 0;
		int m_Height = 0;
		int m_NChannels = 0;
		
	protected:
		int m_MitMapLevel = 0;

	private:

	private:
		bool m_IsLoaded = false;
		bool m_FlipOnLoad = false;
	};
}