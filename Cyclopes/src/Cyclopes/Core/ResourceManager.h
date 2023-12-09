#include "Cyclopes/Core/Base.h"
#include "Cyclopes/Renderer/Texture.h"

namespace cyc {

	class TextureManager
	{
	public:
		static Texture2D* Create2D(const char* name, const char* path);
		static Texture2D* Get2D(const char* name);
		static size_t Count2D();
		static int GetID2D(const char* name);

	public:
		static void _Init();

	private:
		static CycUnorderedMap<CycString, CycShared<Texture2D>> m_2DTextures;
		static CycUnorderedMap<CycString, int> m_2DTextureIDs;
	};

}