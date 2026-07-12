#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIPanel::alGUIPanel(alGUIContext* ct)
	:
	alGUIElement(ct)
{
}

alGUIPanel::~alGUIPanel()
{
}

void alGUIPanel::Draw(float32_t dt)
{
	OnDraw();
	if (m_drawBG)
	{
		m_gs->DrawRectangle(m_buildArea, m_colorTheme->m_panel_bg1, m_colorTheme->m_panel_bg2);
	}

	if (m_visible)
	{
		for (size_t i = 0; i < m_elements.m_size; ++i)
		{
			m_elements.m_data[i]->Draw(dt);
		}
	}
}

void alGUIPanel::Rebuild()
{
	alGUIElement::Rebuild();


	for (size_t i = 0; i < m_elements.m_size; ++i)
	{
		m_elements.m_data[i]->Rebuild();
	}
}

void alGUIPanel::Update(float32_t dt)
{
	if (m_visible)
	{
		if (m_elements.m_size)
		{
			for (size_t i = m_elements.m_size - 1; ; )
			{
				m_elements.m_data[i]->Update(dt);

				if (!i)
					break;
				--i;
			}
		}
	}
	alGUIElement::Update(dt);
}

void alGUIPanel::AddElement(alGUIElement* e)
{
	m_elements.push_back(e);
	e->SetParent(this);
}

