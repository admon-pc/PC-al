#include "al.h"
#include "Image/alImage.h"
#include "GS/alGS.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGSTextureCache::alGSTextureCache(alGS* gs)
	:
	m_gs(gs)
{
}

alGSTextureCache::~alGSTextureCache()
{
	Clear();
}

void alGSTextureCache::Load(alGSTextureCacheNode* node)
{
	AL_ASSERT_ST(node);
	AL_ASSERT_ST(node->m_path);
	if (!node->m_texture)
	{
		alImage* img = alLib::LoadALImage(node->m_path->c_str());
		if (img)
		{
			alGSTextureInfo ti = m_info;
			ti.m_imagePtr = img;
			node->m_texture = m_gs->CreateTexture(&ti);
			alDestroy(img);
		}
	}
}

void alGSTextureCache::Unload(alGSTextureCacheNode* node)
{
	AL_ASSERT_ST(node);
	AL_DESTROY(node->m_texture);
}


alGSTextureCacheNode* alGSTextureCache::GetTexture(const char* filePath, bool load)
{
	alGSTextureCacheNode* _ret = 0;
	AL_ASSERT_ST(filePath);
	auto it = m_map.find(filePath);
	if (it == m_map.end())
	{
		m_map[filePath] = alGSTextureCacheNode();

		it = m_map.find(filePath);
		m_map[filePath].m_path = &it->first;
		_ret = &it->second;
	}
	else
	{
		_ret = &it->second;
	}

	if(load)
		Load(_ret);

	return _ret;
}

void alGSTextureCache::LoadAll()
{
	for (auto & it : m_map)
	{
		Load(&it.second);
	}
}

void alGSTextureCache::UnloadAll()
{
	for (auto& it : m_map)
	{
		Unload(&it.second);
	}
}

void alGSTextureCache::Clear()
{
	UnloadAll();
	m_map.clear();
}

