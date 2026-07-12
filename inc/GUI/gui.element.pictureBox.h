#ifndef _AL_GUIPICBOXH_
#define _AL_GUIPICBOXH_

class alGUIPictureBox: public alGUIElement
{
protected:
	alVec4f m_pictureBoxBuildRect;
	alGSTexture* m_texture = 0;
public:
	alGUIPictureBox(alGUIContext* ct);
	virtual ~alGUIPictureBox();

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	virtual void SetTexture(alGSTexture*);
	alColor m_color;
	bool m_stretch = true;
	alVec4f m_uv = alVec4f(0.f, 0.f, 1.f, 1.f);
};

#endif

