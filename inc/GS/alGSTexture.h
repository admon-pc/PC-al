#ifndef _AL_GSTEXTUREH_
#define _AL_GSTEXTUREH_

enum class alGSTextureType : uint32_t
{
	Texture2D,
	RTT
};

enum class alGSTextureComparisonFunc : uint32_t
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

enum class alGSTextureAddressMode : uint32_t
{
	Wrap,
	Mirror,
	Clamp,
	Border,
	MirrorOnce
};

enum class alGSTextureFilter : uint32_t
{
	// min mag mip / point linear
	PPP,
	PPL,
	PLP,
	PLL,
	LPP,
	LPL,
	LLP,
	LLL,
	ANISOTROPIC,
	// comparison
	CMP_PPP,
	CMP_PPL,
	CMP_PLP,
	CMP_PLL,
	CMP_LPP,
	CMP_LPL,
	CMP_LLP,
	CMP_LLL,
	CMP_ANISOTROPIC,
};

class alGSTextureInfo
{
public:
	alGSTextureInfo() {}
	alGSTextureInfo(alImage* i)
	{
		
	}
	
	~alGSTextureInfo() {}

	alGSTextureType m_textureType = alGSTextureType::Texture2D;
	alGSTextureFilter m_filter = alGSTextureFilter::PPP;
	alGSTextureAddressMode m_addressMode = alGSTextureAddressMode::Wrap;
	alGSTextureComparisonFunc m_comparisonFunc = alGSTextureComparisonFunc::Always;
	uint32_t m_anisotropicLevel = 1;

	uint32_t m_flags = 0;
	enum
	{
		flag_useMipmaps = BIT(0),

		// don't use
		// ....2000 years later
		// WHY?
		flag_fromCache = BIT(1),
	};

	bool IsUseMipMaps()
	{
		return (m_flags & flag_useMipmaps) == flag_useMipmaps;
	}

	void UseMipMaps(bool set)
	{
		if (set)
		{
			m_flags |= flag_useMipmaps;
		}
		else
		{
			if (m_flags & flag_useMipmaps)
				m_flags ^= flag_useMipmaps;
		}
	}

	uint32_t m_width = 0;
	uint32_t m_height = 0;

	alImage* m_imagePtr = nullptr;
};

class alGSTexture
{
protected:
	alGSTextureInfo m_textureInfo;
public:
	alGSTexture() {}
	virtual ~alGSTexture() {}

	alGSTextureInfo* GetGSTextureInfo() { return &m_textureInfo; }
	uint32_t GetWidth() { return m_textureInfo.m_width; }
	uint32_t GetHeight() { return m_textureInfo.m_height; }

	alVec2f GetUV(float32_t x, float32_t y)
	{
		alVec2f uv;

		alVec2f tsz((float32_t)m_textureInfo.m_width, (float32_t)m_textureInfo.m_height);

		uv.x = x * 1.f / tsz.x;
		uv.y = y * 1.f / tsz.y;

		return uv;
	}


};

#endif

