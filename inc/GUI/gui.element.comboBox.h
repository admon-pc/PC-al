#ifndef _AL_GUICOMBOH_
#define _AL_GUICOMBOH_

class alGUIComboBox : public alGUIElement
{
protected:
	alGUIFont* m_font = 0;
	alVec2f m_textPosition;

	void* m_items = 0;
	size_t m_itemsNum = 0;
	uint32_t m_stride = 0;
	uint32_t m_textOffset = 0;
	float32_t m_itemHeight = 0.f;

	size_t m_startIndex = 0;
	size_t m_visibleItemsNum = 0;
	float32_t m_contentHeight = 0.f;
	float32_t m_popupAreaWidth = 0.f;

	alColor m_currColBG;
	alColor m_currColText;

	bool m_active = false;
	bool m_tooManyItems = false;

	alVec4f m_popupArea;
	alVec4f m_popupAreaClip;
	alVec4f m_upBtnRect;
	alVec4f m_downBtnRect;
	//alGUIPopup* m_popup = 0;

	size_t m_itemSelected = 0;
	bool m_isItemMouseHover = false;

	float m_scrollTimer = 0.f;


public:
	alGUIComboBox(alGUIContext* ct);
	virtual ~alGUIComboBox();

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	virtual void OnComboActivate() {};
	virtual void OnComboDeactivate() {};
	virtual void OnComboSelectItem(size_t) {};

	bool IsActive() { return m_active; }
	void SetFont(alGUIFont*);

	// 1. Create your struct.
	// This struct must have
	//    char32_t your_text_variable[fixed_size];
	// 2. Collect all data into array using this struct.
	// 3. Give pointer to this method
	// num - is number of items, size of an array
	// stride is a sizeof of that struct
	// text_offset is offset to your_text_variable inside struct
	// With this way you can store almost unlimited number of items
	void SetItems(void*, size_t num, uint32_t stride, uint32_t text_offset);

	alUnicodeString m_text;
};

#endif

