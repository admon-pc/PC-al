#ifndef _AL_GUIBIH_
#define _AL_GUIBIH_

class alGUIButtonIcon : public alGUIButton
{
	alGUITextureAtlas* m_textureAtlas = 0;
	alColor m_currCol;
	uint32_t m_currIcon = 0;

	bool m_isClicked = false;
	alVec4f m_UV[3];
	
public:
	alGUIButtonIcon(alGUIContext* ct, alGUITextureAtlas*, uint32_t iconIndex, const alVec2f& position, const alVec2f& size);
	virtual ~alGUIButtonIcon();

	uint32_t m_iconIndexBase = 0;
	uint32_t m_iconIndexMouseHover = 0;
	uint32_t m_iconIndexPress = 0;

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	bool m_useText = false;
	alVec2i m_textIndent;

};


#endif

