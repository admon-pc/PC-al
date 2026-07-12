#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIPictureBox::alGUIPictureBox(alGUIContext* ct)
	:
	alGUIElement(ct)
{
	m_color = ColorWhite;
}

alGUIPictureBox::~alGUIPictureBox()
{
}

void alGUIPictureBox::Draw(float32_t dt)
{
	AL_ASSERT_ST(m_texture);
	OnDraw();

	m_gs->SetScissorRect(m_clipArea);
	m_gs->DrawRectangle(
		m_pictureBoxBuildRect,
		m_color,
		m_color,
		m_texture,
		&m_uv);
}

void alGUIPictureBox::Update(float32_t dt)
{
	alGUIElement::Update(dt);
}

void alGUIPictureBox::Rebuild()
{
	alGUIElement::Rebuild();

	m_pictureBoxBuildRect = m_buildArea;
}

void alGUIPictureBox::SetTexture(alGSTexture* t)
{
	AL_ASSERT_ST(t);
	m_texture = t;
}

