#ifndef _AL_GSTEXTURECACHEH_
#define _AL_GSTEXTURECACHEH_

#include <map>
#include <string>

struct alGSTextureCacheNode
{
	alGSTexture* m_texture = 0;
	const std::string* m_path = 0;
};

class alGSTextureCache
{
	std::map<std::string, alGSTextureCacheNode> m_map;
	alGS* m_gs = 0;
public:
	alGSTextureCache(alGS*);
	~alGSTextureCache();

	alGSTextureCacheNode* GetTexture(const char* filePath, bool load = true);

	void Load(alGSTextureCacheNode*);
	void Unload(alGSTextureCacheNode*);

	void LoadAll();
	void UnloadAll();
	
	void Clear();

	alGSTextureInfo m_info;
};

#endif

