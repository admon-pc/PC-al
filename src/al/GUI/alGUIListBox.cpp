#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIListBox::alGUIListBox(alGUIContext* ct)
	:
	alGUIElement(ct)
{
	SetFont(alLib::GetDefaultFont());
	m_itemHeight = 10.f;
}

alGUIListBox::~alGUIListBox()
{
}

void alGUIListBox::Draw(float32_t dt)
{
	m_gs->SetScissorRect(m_clipArea);
	OnDraw();
	auto input = alLib::GetInput();

	m_gs->DrawRectangle(
		m_buildArea,
		m_colorTheme->m_list_bg);

	m_isItemMouseHover = false;

	if (m_items && m_itemsNum && m_font)
	{
		float posY = m_buildArea.y;

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

		m_gs->SetScissorRect(m_listClipArea);

		uint8_t* ptr = (uint8_t*)m_items;
		size_t index = m_startIndex;
		size_t drawnNum = 1;
		for (size_t i = 0; i < m_visibleItemsNum; ++i)
		{
			uint32_t* flags = (uint32_t*)(&ptr[index * m_stride] + m_flagsOffset);

			char32_t* itemTextPtr = (char32_t*)(&ptr[index * m_stride] + m_textOffset);
			char32_t* itemText = itemTextPtr;
			size_t itemTextSz = 0;
			while (*itemTextPtr)
			{
				++itemTextSz;
				++itemTextPtr;
			}
				

			alVec4f itemRect;
			itemRect.x = m_listClipArea.x;
			itemRect.y = posY;
			itemRect.z = m_listClipArea.z;
			itemRect.w = itemRect.y + m_itemHeight-1;

			auto textColor = m_colorTheme->m_popup_itemText;

			bool pointIn = false;
			if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
				input->m_cursorCoordsForGUI.y, itemRect))
			{
				pointIn = true;
				if (!m_isItemMouseHover)
				{
					m_isItemMouseHover = true;
					m_itemSelected = index;
				}
			}

			if (pointIn || (*flags & flag_selected))
			{
				m_gs->DrawRectangle(
					itemRect,
					m_colorTheme->m_popup_bg1_mouseHover,
					m_colorTheme->m_popup_bg2_mouseHover);
				textColor = m_colorTheme->m_popup_itemText_mouseHover;
			}
				
			if(itemTextSz)
				m_gs->DrawText(itemText, itemTextSz, m_font,
				alVec2f(m_listClipArea.x, posY),
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

void alGUIListBox::Update(float32_t dt)
{
	auto input = alLib::GetInput();

	alGUIElement::Update(dt);
	
	if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_buildArea))
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
					size_t lastIndex = m_itemsNum - m_visibleItemsNum+2;
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
					size_t lastIndex = m_itemsNum - m_visibleItemsNum + 2;
					++m_startIndex;
					if (m_startIndex > lastIndex)
						m_startIndex = lastIndex;
				}
			}
		}

		if (input->m_isLMBDown && m_isItemMouseHover)
		{
			OnListSelectItem(m_itemSelected);
			m_context->m_activeElement = 0;
		}
	}
}

void alGUIListBox::Rebuild()
{
	alGUIElement::Rebuild();
	m_contentHeight = m_itemsNum * m_itemHeight;

	m_listClipArea = m_buildArea;

	auto areaHeight = m_buildArea.w - m_buildArea.y;
	if (!areaHeight)
		areaHeight = 10.f;

	m_visibleItemsNum = areaHeight / m_itemHeight;
	m_tooManyItems = false;
	if(m_itemsNum > m_visibleItemsNum)
		m_tooManyItems = true;

	if (m_tooManyItems)
	{
		m_upBtnRect.x = m_listClipArea.x;
		m_upBtnRect.y = m_listClipArea.y;
		m_upBtnRect.z = m_listClipArea.z;
		m_upBtnRect.w = m_upBtnRect.y + 10.f;
		

		m_downBtnRect.x = m_listClipArea.x;
		m_downBtnRect.y = m_listClipArea.w - 10.f;
		m_downBtnRect.z = m_listClipArea.z;
		m_downBtnRect.w = m_listClipArea.w;

		m_listClipArea.y = m_upBtnRect.w;
		m_listClipArea.w = m_downBtnRect.y;
	}

	
}

void alGUIListBox::SetFont(alGUIFont* f)
{
	m_font = f;
	m_itemHeight = m_font->m_maxHeight;
	if (!m_itemHeight)
		m_itemHeight = 10.f;
}

void alGUIListBox::SetItems(
	void* ptr, 
	size_t num, 
	uint32_t stride, 
	uint32_t text_offset, 
	uint32_t flags_offset)
{
	m_items = ptr;
	m_itemsNum = num;
	m_stride = stride;
	m_textOffset = text_offset;
	m_flagsOffset = flags_offset;
}

