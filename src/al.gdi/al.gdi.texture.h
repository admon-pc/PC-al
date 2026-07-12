#ifndef _ALGSGDIT_H_
#define _ALGSGDIT_H_

class alGSGDITexture : public alGSTexture
{
public:
	alGSGDITexture();
	virtual ~alGSGDITexture();

	bool Init();

	Gdiplus::Bitmap* m_bitmap = 0;
	alGSTextureInfo m_ti;
};

#endif
