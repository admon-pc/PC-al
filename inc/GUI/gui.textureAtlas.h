#ifndef _AL_GUITEXTUREATLASH_
#define _AL_GUITEXTUREATLASH_

// just texture and UV points.
class alGUITextureAtlas
{
	alGSTexture* m_texture = nullptr;
	alArray<alVec4f> m_UVs;
public:
	alGUITextureAtlas(alGSTexture* t);
	~alGUITextureAtlas();

	// Set texture first!
	// size - optional
	uint32_t AddUV(const alVec2u& lt, const alVec2u& size);
	const alVec4f& GetUV(uint32_t index);
	alGSTexture* GetTexture() { return m_texture; }
};

#endif

