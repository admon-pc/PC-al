#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGSTexture.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUITextureAtlas::alGUITextureAtlas(alGSTexture* t)
	:
	m_texture(t)
{
}

alGUITextureAtlas::~alGUITextureAtlas()
{
}

// Set texture first!
uint32_t alGUITextureAtlas::AddUV(const alVec2u& lt, const alVec2u& size)
{
	AL_ASSERT_ST(m_texture);

	alVec2f _lt(lt.x, lt.y);
	alVec2f _rb(lt.x + size.x, lt.y + size.y);

	if (m_texture)
	{
		alVec2f tsz((float32_t)m_texture->GetWidth(), (float32_t)m_texture->GetHeight());
		float32_t mulX = 1.f / (float32_t)tsz.x;
		float32_t mulY = 1.f / (float32_t)tsz.y;

		_lt.x = lt.x * mulX;
		_lt.y = lt.y * mulY;
		_rb.x = (_rb.x ) * mulX;
		_rb.y = (_rb.y ) * mulY;
	}

	m_UVs.push_back(alVec4f(_lt.x, _lt.y, _rb.x, _rb.y));
	return m_UVs.size() - 1;
}

const alVec4f& alGUITextureAtlas::GetUV(uint32_t index)
{
	AL_ASSERT_ST(m_UVs.m_size);
	AL_ASSERT_ST(index < m_UVs.m_size);
	return m_UVs.m_data[index];
}
