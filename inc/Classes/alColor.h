#ifndef _AL_COLOR_H_
#define _AL_COLOR_H_

// I don't know how to call it
#define MI_ColorDivider 0.00392156862745f

class alColorBytes
{
public:
	alColorBytes()
		:
		r(0),
		g(0),
		b(0),
		a(255)
	{}
	alColorBytes(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
		:
		r(R),
		g(G),
		b(B),
		a(A)
	{}

	alColorBytes(uint32_t uint_data) // 0xFF112233 ARGB
	{
		r = static_cast<uint8_t>(uint_data >> 16u);
		g = static_cast<uint8_t>(uint_data >> 8u);
		b = static_cast<uint8_t>(uint_data);
		a = static_cast<uint8_t>(uint_data >> 24u);
	}

	uint8_t r, g, b, a;

	bool	operator==(const alColorBytes& v) const
	{
		if (r != v.r) return false;
		if (g != v.g) return false;
		if (b != v.b) return false;
		if (a != v.a) return false;
		return true;
	}

	bool	operator!=(const alColorBytes& v) const
	{
		if (r != v.r) return true;
		if (g != v.g) return true;
		if (b != v.b) return true;
		if (a != v.a) return true;
		return false;
	}
};

class alColor
{
public:

	float32_t	m_data[4u];

	alColor()
	{
		m_data[0u] = m_data[1u] = m_data[2u] = 0.f;
		m_data[3u] = 1.f;
	}

	alColor(float32_t v)
	{
		m_data[0u] = m_data[1u] = m_data[2u] = v;
		m_data[3u] = 1.f;
	}

	alColor(float32_t r, float32_t g, float32_t b, float32_t a = 1.f)
	{
		m_data[0u] = r;
		m_data[1u] = g;
		m_data[2u] = b;
		m_data[3u] = a;
	}

	alColor(int32_t r, int32_t g, int32_t b, int32_t a = 255)
	{
		this->SetAsByteAlpha(a);
		this->SetAsByteRed(r);
		this->SetAsByteGreen(g);
		this->setAsByteBlue(b);
	}

	alColor(uint32_t uint_data) // 0xFF112233 ARGB
	{
		SetAsInteger(uint_data);
	}

	alVec4f GetV4f()const { return alVec4f(m_data[0], m_data[1], m_data[2], m_data[3]); }

	float32_t X() { return m_data[0]; }
	float32_t Y() { return m_data[1]; }
	float32_t Z() { return m_data[2]; }
	float32_t W() { return m_data[3]; }

	const float32_t* Data() const { return &m_data[0u]; }

	const float32_t GetRed() const { return m_data[0u]; }
	const float32_t GetGreen() const { return m_data[1u]; }
	const float32_t GetBlue() const { return m_data[2u]; }
	const float32_t GetAlpha() const { return m_data[3u]; }

	const uint8_t GetAsByteRed() const { return static_cast<uint8_t>(m_data[0u] * 255.); }
	const uint8_t GetAsByteGreen() const { return static_cast<uint8_t>(m_data[1u] * 255.); }
	const uint8_t GetAsByteBlue() const { return static_cast<uint8_t>(m_data[2u] * 255.); }
	const uint8_t GetAsByteAlpha() const { return static_cast<uint8_t>(m_data[3u] * 255.); }

	bool	operator==(const alColor& v) const
	{
		if (m_data[0] != v.m_data[0]) return false;
		if (m_data[1] != v.m_data[1]) return false;
		if (m_data[2] != v.m_data[2]) return false;
		if (m_data[3] != v.m_data[3]) return false;
		return true;
	}

	bool	operator!=(const alColor& v) const
	{
		if (m_data[0] != v.m_data[0]) return true;
		if (m_data[1] != v.m_data[1]) return true;
		if (m_data[2] != v.m_data[2]) return true;
		if (m_data[3] != v.m_data[3]) return true;
		return false;
	}

	void Normalize()
	{
		float32_t len = std::sqrt((m_data[0] * m_data[0]) + (m_data[1] * m_data[1]) + (m_data[2] * m_data[2]));
		if (len > 0)
		{
			len = 1.0f / len;
		}
		m_data[0] *= len;
		m_data[1] *= len;
		m_data[2] *= len;
	}

	// 0xff112233
	uint32_t getAsInteger()
	{
		return AL_MAKEFOURCC(
			this->GetAsByteBlue(),
			this->GetAsByteGreen(),
			this->GetAsByteRed(),
			this->GetAsByteAlpha()
		);
	}

	void SetAlpha(float32_t v) { m_data[3u] = v; }
	void SetRed(float32_t v) { m_data[0u] = v; }
	void SetGreen(float32_t v) { m_data[1u] = v; }
	void SetBlue(float32_t v) { m_data[2u] = v; }

	void SetAsByteAlpha(int32_t v) { m_data[3u] = static_cast<float32_t>(v) * MI_ColorDivider; }
	void SetAsByteRed(int32_t v) { m_data[0u] = static_cast<float32_t>(v) * MI_ColorDivider; }
	void SetAsByteGreen(int32_t v) { m_data[1u] = static_cast<float32_t>(v) * MI_ColorDivider; }
	void setAsByteBlue(int32_t v) { m_data[2u] = static_cast<float32_t>(v) * MI_ColorDivider; }

	void SetAsInteger(uint32_t v)
	{
		this->SetAsByteRed(static_cast<uint8_t>(v >> 16u));
		this->SetAsByteGreen(static_cast<uint8_t>(v >> 8u));
		this->setAsByteBlue(static_cast<uint8_t>(v));
		this->SetAsByteAlpha(static_cast<uint8_t>(v >> 24u));
	}

	void Set(float32_t r, float32_t g, float32_t b, float32_t a = 1.)
	{
		SetAlpha(a);
		SetRed(r);
		SetGreen(g);
		SetBlue(b);
	}

	void Set(float32_t v, float32_t a = 1.)
	{
		SetAlpha(a);
		SetRed(v);
		SetGreen(v);
		SetBlue(v);
	}

	void Set(const alColor& other)
	{
		*this = other;
	}
};

const alColor ColorTransparent = (uint32_t)0xF0F8FF;

//	HTML colors
const alColor ColorAliceBlue = 0xffF0F8FF;
const alColor ColorAntiqueWhite = 0xffFAEBD7;
const alColor ColorAqua = 0xff00FFFF;
const alColor ColorAquamarine = 0xff7FFFD4;
const alColor ColorAzure = 0xffF0FFFF;
const alColor ColorBeige = 0xffF5F5DC;
const alColor ColorBisque = 0xffFFE4C4;
const alColor ColorBlack = 0xff000000;
const alColor ColorBlanchedAlmond = 0xffFFEBCD;
const alColor ColorBlue = 0xff0000FF;
const alColor ColorBlueViolet = 0xff8A2BE2;
const alColor ColorBrown = 0xffA52A2A;
const alColor ColorBurlyWood = 0xffDEB887;
const alColor ColorCadetBlue = 0xff5F9EA0;
const alColor ColorChartreuse = 0xff7FFF00;
const alColor ColorChocolate = 0xffD2691E;
const alColor ColorCoral = 0xffFF7F50;
const alColor ColorCornflowerBlue = 0xff6495ED;
const alColor ColorCornsilk = 0xffFFF8DC;
const alColor ColorCrimson = 0xffDC143C;
const alColor ColorCyan = 0xff00FFFF;
const alColor ColorDarkBlue = 0xff00008B;
const alColor ColorDarkCyan = 0xff008B8B;
const alColor ColorDarkGoldenRod = 0xffB8860B;
const alColor ColorDarkGray = 0xffA9A9A9;
const alColor ColorDarkGrey = 0xffA9A9A9;
const alColor ColorDarkGreen = 0xff006400;
const alColor ColorDarkKhaki = 0xffBDB76B;
const alColor ColorDarkMagenta = 0xff8B008B;
const alColor ColorDarkOliveGreen = 0xff556B2F;
const alColor ColorDarkOrange = 0xffFF8C00;
const alColor ColorDarkOrchid = 0xff9932CC;
const alColor ColorDarkRed = 0xff8B0000;
const alColor ColorDarkSalmon = 0xffE9967A;
const alColor ColorDarkSeaGreen = 0xff8FBC8F;
const alColor ColorDarkSlateBlue = 0xff483D8B;
const alColor ColorDarkSlateGray = 0xff2F4F4F;
const alColor ColorDarkSlateGrey = 0xff2F4F4F;
const alColor ColorDarkTurquoise = 0xff00CED1;
const alColor ColorDarkViolet = 0xff9400D3;
const alColor ColorDeepPink = 0xffFF1493;
const alColor ColorDeepSkyBlue = 0xff00BFFF;
const alColor ColorDimGray = 0xff696969;
const alColor ColorDimGrey = 0xff696969;
const alColor ColorDodgerBlue = 0xff1E90FF;
const alColor ColorFireBrick = 0xffB22222;
const alColor ColorFloralWhite = 0xffFFFAF0;
const alColor ColorForestGreen = 0xff228B22;
const alColor ColorFuchsia = 0xffFF00FF;
const alColor ColorGainsboro = 0xffDCDCDC;
const alColor ColorGhostWhite = 0xffF8F8FF;
const alColor ColorGold = 0xffFFD700;
const alColor ColorGoldenRod = 0xffDAA520;
const alColor ColorGray = 0xff808080;
const alColor ColorGrey = 0xff808080;
const alColor ColorGreen = 0xff008000;
const alColor ColorGreenYellow = 0xffADFF2F;
const alColor ColorHoneyDew = 0xffF0FFF0;
const alColor ColorHotPink = 0xffFF69B4;
const alColor ColorIndianRed = 0xffCD5C5C;
const alColor ColorIndigo = 0xff4B0082;
const alColor ColorIvory = 0xffFFFFF0;
const alColor ColorKhaki = 0xffF0E68C;
const alColor ColorLavender = 0xffE6E6FA;
const alColor ColorLavenderBlush = 0xffFFF0F5;
const alColor ColorLawnGreen = 0xff7CFC00;
const alColor ColorLemonChiffon = 0xffFFFACD;
const alColor ColorLightBlue = 0xffADD8E6;
const alColor ColorLightCoral = 0xffF08080;
const alColor ColorLightCyan = 0xffE0FFFF;
const alColor ColorLightGoldenRodYellow = 0xffFAFAD2;
const alColor ColorLightGray = 0xffD3D3D3;
const alColor ColorLightGrey = 0xffD3D3D3;
const alColor ColorLightGreen = 0xff90EE90;
const alColor ColorLightPink = 0xffFFB6C1;
const alColor ColorLightSalmon = 0xffFFA07A;
const alColor ColorLightSeaGreen = 0xff20B2AA;
const alColor ColorLightSkyBlue = 0xff87CEFA;
const alColor ColorLightSlateGray = 0xff778899;
const alColor ColorLightSlateGrey = 0xff778899;
const alColor ColorLightSteelBlue = 0xffB0C4DE;
const alColor ColorLightYellow = 0xffFFFFE0;
const alColor ColorLime = 0xff00FF00;
const alColor ColorLimeGreen = 0xff32CD32;
const alColor ColorLinen = 0xffFAF0E6;
const alColor ColorMagenta = 0xffFF00FF;
const alColor ColorMaroon = 0xff800000;
const alColor ColorMediumAquaMarine = 0xff66CDAA;
const alColor ColorMediumBlue = 0xff0000CD;
const alColor ColorMediumOrchid = 0xffBA55D3;
const alColor ColorMediumPurple = 0xff9370DB;
const alColor ColorMediumSeaGreen = 0xff3CB371;
const alColor ColorMediumSlateBlue = 0xff7B68EE;
const alColor ColorMediumSpringGreen = 0xff00FA9A;
const alColor ColorMediumTurquoise = 0xff48D1CC;
const alColor ColorMediumVioletRed = 0xffC71585;
const alColor ColorMidnightBlue = 0xff191970;
const alColor ColorMintCream = 0xffF5FFFA;
const alColor ColorMistyRose = 0xffFFE4E1;
const alColor ColorMoccasin = 0xffFFE4B5;
const alColor ColorNavajoWhite = 0xffFFDEAD;
const alColor ColorNavy = 0xff000080;
const alColor ColorOldLace = 0xffFDF5E6;
const alColor ColorOlive = 0xff808000;
const alColor ColorOliveDrab = 0xff6B8E23;
const alColor ColorOrange = 0xffFFA500;
const alColor ColorOrangeRed = 0xffFF4500;
const alColor ColorOrchid = 0xffDA70D6;
const alColor ColorPaleGoldenRod = 0xffEEE8AA;
const alColor ColorPaleGreen = 0xff98FB98;
const alColor ColorPaleTurquoise = 0xffAFEEEE;
const alColor ColorPaleVioletRed = 0xffDB7093;
const alColor ColorPapayaWhip = 0xffFFEFD5;
const alColor ColorPeachPuff = 0xffFFDAB9;
const alColor ColorPeru = 0xffCD853F;
const alColor ColorPink = 0xffFFC0CB;
const alColor ColorPlum = 0xffDDA0DD;
const alColor ColorPowderBlue = 0xffB0E0E6;
const alColor ColorPurple = 0xff800080;
const alColor ColorRebeccaPurple = 0xff663399;
const alColor ColorRed = 0xffFF0000;
const alColor ColorRosyBrown = 0xffBC8F8F;
const alColor ColorRoyalBlue = 0xff4169E1;
const alColor ColorSaddleBrown = 0xff8B4513;
const alColor ColorSalmon = 0xffFA8072;
const alColor ColorSandyBrown = 0xffF4A460;
const alColor ColorSeaGreen = 0xff2E8B57;
const alColor ColorSeaShell = 0xffFFF5EE;
const alColor ColorSienna = 0xffA0522D;
const alColor ColorSilver = 0xffC0C0C0;
const alColor ColorSkyBlue = 0xff87CEEB;
const alColor ColorSlateBlue = 0xff6A5ACD;
const alColor ColorSlateGray = 0xff708090;
const alColor ColorSlateGrey = 0xff708090;
const alColor ColorSnow = 0xffFFFAFA;
const alColor ColorSpringGreen = 0xff00FF7F;
const alColor ColorSteelBlue = 0xff4682B4;
const alColor ColorTan = 0xffD2B48C;
const alColor ColorTeal = 0xff008080;
const alColor ColorThistle = 0xffD8BFD8;
const alColor ColorTomato = 0xffFF6347;
const alColor ColorTurquoise = 0xff40E0D0;
const alColor ColorViolet = 0xffEE82EE;
const alColor ColorWheat = 0xffF5DEB3;
const alColor ColorWhite = 0xffffffff;
const alColor ColorWhiteSmoke = 0xffF5F5F5;
const alColor ColorYellow = 0xffFFFF00;
const alColor ColorYellowGreen = 0xff9ACD32;


#endif
