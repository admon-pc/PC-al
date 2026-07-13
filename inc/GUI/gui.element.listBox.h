#ifndef _AL_GUILISTBXH_
#define _AL_GUILISTBXH_

// Almost same as combo box
class alGUIListBox : public alGUIElement
{
protected:
	alGUIFont* m_font = 0;

	void* m_items = 0;
	size_t m_itemsNum = 0;
	uint32_t m_stride = 0;
	uint32_t m_textOffset = 0;
	uint32_t m_flagsOffset = 0;
	float32_t m_itemHeight = 0.f;

	size_t m_startIndex = 0;
	size_t m_visibleItemsNum = 0;
	float32_t m_contentHeight = 0.f;

	alColor m_currColBG;
	alColor m_currColText;

	bool m_tooManyItems = false;

	alVec4f m_upBtnRect;
	alVec4f m_downBtnRect;

	alVec4f m_listClipArea;

	size_t m_itemSelected = 0;
	bool m_isItemMouseHover = false;

	float m_scrollTimer = 0.f;

public:
	alGUIListBox(alGUIContext* ct);
	virtual ~alGUIListBox();

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	virtual void OnListSelectItem(size_t) {};

	void SetFont(alGUIFont*);

	enum
	{
		flag_selected = 1,
	};

	// see same method in combobox
	void SetItems(void*, size_t num, uint32_t stride, uint32_t text_offset, uint32_t flags_offset);
};


#endif

