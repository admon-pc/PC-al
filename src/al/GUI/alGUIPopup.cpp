#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"
#include "GUI/gui.popup.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIPopup::alGUIPopup(alGUIContext*c)
	:
	alGUIElement(c)
{
	m_font = alLib::GetDefaultFont();
	m_colorTheme = alLib::GetDefaultColorTheme();
}

alGUIPopup::~alGUIPopup()
{
}

void alGUIPopup::SetPositionAndSize(float32_t posX, float32_t posY, float32_t szX, float32_t szY)
{
	m_buildArea.x = posX;
	m_buildArea.y = posY;
	m_buildArea.z = m_buildArea.x + szX;
	m_buildArea.w = m_buildArea.y + szY;
}

void alGUIPopup::Rebuild()
{
}

void alGUIPopup::Draw(float32_t dt)
{
	m_gs->SetScissorRect(m_buildArea);

	m_gs->DrawRectangle(
		m_buildArea,
		m_colorTheme->m_popup_bg1,
		m_colorTheme->m_popup_bg2);
}

void alGUIPopup::Update(float32_t dt)
{
	auto input = alLib::GetInput();
//	printf("1");
	if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_buildArea))
	{
	}
	else
	{
	//	printf("2");
		if (input->m_isLMBDown)
		{
			m_context->m_activePopup = 0;
	//		printf("\n");
		}
	}
}

void alGUIPopup::Activate()
{
	m_context->m_activePopup = this;
}
