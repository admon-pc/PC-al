#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIComboBox::alGUIComboBox(alGUIContext* ct)
	:
	alGUIElement(ct)
{
	//m_popup = alCreate<alGUIPopup>(ct);
	SetFont(alLib::GetDefaultFont());
	m_itemHeight = 10.f;
}

alGUIComboBox::~alGUIComboBox()
{
	//AL_DESTROY(m_popup);
}

void alGUIComboBox::Draw(float32_t dt)
{
	m_gs->SetScissorRect(m_clipArea);
	OnDraw();
	m_currColBG = m_colorTheme->m_combo_bg_enabled;
	m_currColText = m_colorTheme->m_combo_textColor_enabled;

	if (m_isLMBClicked)
	{
		m_isLMBClicked = false;
		m_currColBG = m_colorTheme->m_combo_bg_press;
		m_currColText = m_colorTheme->m_combo_textColor_press;
		//m_popup->Activate();
	}
	else
	{
		if (m_isCursorInSensorArea || m_active)
		{
			m_currColBG = m_colorTheme->m_combo_bg_mouseHover;
			m_currColText = m_colorTheme->m_combo_textColor_mouseHover;
		}
	}
	auto input = alLib::GetInput();

	m_gs->DrawRectangle(
		m_buildArea,
		m_currColBG,
		m_currColBG);

	auto textSz = m_text.Size();
	if (m_font && textSz)
	{
		m_gs->DrawText(m_text.Data(), textSz, m_font,
			m_textPosition, m_currColText);
	}

	if (m_active)
	{
		m_isItemMouseHover = false;

		m_gs->SetScissorRect(m_popupArea);
		
		m_gs->DrawRectangle(
			m_popupArea,
			m_colorTheme->m_popup_bg1,
			m_colorTheme->m_popup_bg2);

		if (m_items && m_itemsNum && m_font)
		{
			float posY = m_popupArea.y;

			if (m_tooManyItems)
			{
				m_gs->DrawRectangle(
					m_upBtnRect,
					ColorRed);

				m_gs->DrawRectangle(
					m_downBtnRect,
					ColorBlue);

				posY += 10.f;
			}
			m_gs->SetScissorRect(m_popupAreaClip);

			uint8_t* ptr = (uint8_t*)m_items;
			size_t index = m_startIndex;
			size_t drawnNum = 1;
			for (size_t i = 0; i < m_visibleItemsNum; ++i)
			{
				char32_t* itemTextPtr = (char32_t*)(&ptr[index * m_stride] + m_textOffset);
				char32_t* itemText = itemTextPtr;
				size_t itemTextSz = 0;
				while (*itemTextPtr)
				{
					++itemTextSz;
					++itemTextPtr;
				}
				

				alVec4f itemRect;
				itemRect.x = m_popupArea.x;
				itemRect.y = posY;
				itemRect.z = m_popupAreaWidth;
				itemRect.w = itemRect.y + m_itemHeight-1;

				auto textColor = m_colorTheme->m_popup_itemText;

				if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
					input->m_cursorCoordsForGUI.y, itemRect))
				{
					m_gs->DrawRectangle(
						itemRect,
						m_colorTheme->m_popup_bg1_mouseHover,
						m_colorTheme->m_popup_bg2_mouseHover);
					textColor = m_colorTheme->m_popup_itemText_mouseHover;

					m_isItemMouseHover = true;
					m_itemSelected = index;
				}
				
				if(itemTextSz)
					m_gs->DrawText(itemText, itemTextSz, m_font,
					alVec2f(m_popupArea.x, posY),
						textColor);

				posY += m_itemHeight;
				//ptr += m_stride;
				++index;
				if (index == m_itemsNum)
					break;

				if (drawnNum == m_visibleItemsNum)
					break;
				++drawnNum;
			}
		}
	}
}

void alGUIComboBox::Update(float32_t dt)
{
	auto input = alLib::GetInput();

	alGUIElement::Update(dt);
	
	if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_popupArea))
	{
		if (m_tooManyItems)
		{

			if (input->m_wheelDelta)
			{
				if (input->m_wheelDelta > 0.f)
				{
					//down
					if (m_startIndex)
						--m_startIndex;
				}
				if (input->m_wheelDelta < 0.f)
				{
					size_t lastIndex = m_itemsNum - m_visibleItemsNum+1;
					//move up
					++m_startIndex;
					if (m_startIndex > lastIndex)
						m_startIndex = lastIndex;
				}
			}

			if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
				input->m_cursorCoordsForGUI.y, m_upBtnRect))
			{
				m_scrollTimer += dt;
				if (m_scrollTimer > 0.03)
				{
					m_scrollTimer = 0.f;
					if (m_startIndex)
						--m_startIndex;
				}
			}
			else if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
				input->m_cursorCoordsForGUI.y, m_downBtnRect))
			{
				m_scrollTimer += dt;
				if (m_scrollTimer > 0.03)
				{
					m_scrollTimer = 0.f;
					size_t lastIndex = m_itemsNum - m_visibleItemsNum + 1;
					++m_startIndex;
					if (m_startIndex > lastIndex)
						m_startIndex = lastIndex;
				}
			}
		}

		if (input->m_isLMBDown && m_isItemMouseHover)
		{
			OnComboSelectItem(m_itemSelected);
			OnComboDeactivate();
			m_active = false;
			m_context->m_activeElement = 0;
		}
	}
	else
	{
		if (input->m_isLMBDown && m_context->m_activeElement==this)
		{
			m_context->m_activeElement = 0;
			m_active = false;
			OnComboDeactivate();
			return;
		}
	}

	if (m_isLMBClicked)
	{
		m_context->m_activeElement = this;
		m_active = true;
		OnComboActivate();
	}
}

void alGUIComboBox::Rebuild()
{
	alGUIElement::Rebuild();
	m_textPosition.x = m_buildArea.x;
	m_textPosition.y = m_buildArea.y;
	m_popupAreaWidth = m_buildArea.z - m_buildArea.x;
	auto textSz = m_text.Size();
	if (m_font && textSz)
	{
		auto textW = m_font->GetTextLength(m_text.Data(), textSz);
		auto textH = m_font->m_maxHeight;

		alVec2f buttonHalfSize;
		buttonHalfSize.x = ((m_buildArea.z - m_buildArea.x) * 0.5f);
		buttonHalfSize.y = ((m_buildArea.w - m_buildArea.y) * 0.5f);

		//m_textPosition.x += buttonHalfSize.x - textW * 0.5f;
		m_textPosition.y += buttonHalfSize.y - textH * 0.5f;

		
	}


	m_contentHeight = m_itemsNum * m_itemHeight;

	m_popupArea.x = m_buildArea.x;
	m_popupArea.y = m_buildArea.w;
	m_popupArea.z = m_popupArea.x + m_popupAreaWidth;
	m_popupArea.w = m_popupArea.y + m_contentHeight;
	m_tooManyItems = false;
	auto areaHeight = m_popupArea.w - m_popupArea.y;
	if (areaHeight > 100.f)
	{
		if (m_popupArea.w > m_parent->m_buildArea.w - 20)
		{
			m_popupArea.w = m_parent->m_buildArea.w;
			areaHeight = m_popupArea.w - m_popupArea.y;
			m_tooManyItems = true;
		}
	}

	m_visibleItemsNum = 0;
	if(areaHeight > 0.f)
		m_visibleItemsNum = areaHeight / m_itemHeight;
	if (m_font && textSz)
	{
		uint8_t* ptr = (uint8_t*)m_items;
		size_t index = m_startIndex;
		for (size_t i = 0; i < m_visibleItemsNum; ++i)
		{
			char32_t* itemTextPtr = (char32_t*)(&ptr[index * m_stride] + m_textOffset);
			char32_t* itemText = itemTextPtr;
			size_t itemTextSz = 0;
			while (*itemTextPtr)
			{
				++itemTextSz;
				++itemTextPtr;
			}

			auto textW = m_font->GetTextLength(itemText, itemTextSz);
			if (textW > m_popupAreaWidth)
				m_popupAreaWidth = textW;

			++index;
			if (index == m_itemsNum)
				break;
		}
	}
	m_popupArea.z = m_popupArea.x + m_popupAreaWidth;

	m_popupAreaClip = m_popupArea;

	if (m_tooManyItems)
	{
		m_upBtnRect.x = m_popupArea.x;
		m_upBtnRect.y = m_popupArea.y;
		m_upBtnRect.z = m_popupArea.z;
		m_upBtnRect.w = m_upBtnRect.y + 10.f;
		

		m_downBtnRect.x = m_popupArea.x;
		m_downBtnRect.y = m_popupArea.w - 10.f;
		m_downBtnRect.z = m_popupArea.z;
		m_downBtnRect.w = m_popupArea.w;

		m_popupAreaClip.y = m_upBtnRect.w;
		m_popupAreaClip.w = m_downBtnRect.y;
	}

	
}

void alGUIComboBox::SetFont(alGUIFont* f)
{
	m_font = f;
	m_itemHeight = m_font->m_maxHeight;
	if (!m_itemHeight)
		m_itemHeight = 10.f;
}

void alGUIComboBox::SetItems(
	void* ptr, size_t num, uint32_t stride, uint32_t text_offset)
{
	m_items = ptr;
	m_itemsNum = num;
	m_stride = stride;
	m_textOffset = text_offset;
}

