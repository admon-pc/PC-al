#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGSTexture.h"
#include "GS/alGS.h"
#include "System/alSystemWindow.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIElement::alGUIElement(alGUIContext* ctx, const alVec2f& position, const alVec2f& size)
	:
	m_context(ctx),
	m_position(position),
	m_size(size)
{
	m_gs = m_context->GetGS();
	m_input = alLib::GetInput();
	m_colorTheme = alLib::GetDefaultColorTheme();

	m_position = position;
	m_size = size;

	/*m_buildAreaOnCreation.x = position.x;
	m_buildAreaOnCreation.y = position.y;
	m_buildAreaOnCreation.z = position.x + size.x;
	m_buildAreaOnCreation.w = position.y + size.y;*/
}

void alGUIElement::Update(float32_t dt)
{
	m_isCursorInSensorArea = alMath::PointInRect(
		m_input->m_cursorCoordsForGUI.x, 
		m_input->m_cursorCoordsForGUI.y, 
		m_sensorArea);

	if (m_input->m_isLMBUp)
	{
		if (m_isLMBClicked)
		{
			m_isLMBClicked = false;
			if (m_isCursorInSensorArea)
				OnLMBRelease();
		}
		m_context->m_elementInMouseFocus = 0;
	}
	if (m_input->m_isRMBUp)
	{
		if (m_isRMBClicked)
		{
			m_isRMBClicked = false;
			if (m_isCursorInSensorArea)
				OnRMBRelease();
		}
		m_context->m_elementInMouseFocus = 0;
	}
	if (m_input->m_isMMBUp)
	{
		if (m_isMMBClicked)
		{
			m_isMMBClicked = false;
			if (m_isCursorInSensorArea)
				OnMMBRelease();
		}
		m_context->m_elementInMouseFocus = 0;
	}

	if (m_isCursorInSensorArea)
	{
		if (!m_context->m_isElementUnderCursor)
		{
			m_context->m_isElementUnderCursor = true;

			if (m_context->m_elementUnderCursor != this)
			{
				if (m_context->m_elementUnderCursor)
					m_context->m_elementUnderCursor->OnMouseLeave();

				OnMouseEnter();
			}
			else
			{
				if (m_input->m_mouseDelta.x || m_input->m_mouseDelta.y)
				{
					OnMouseMove();
				}

				if (m_input->m_isLMBDown)
				{
					OnLMBHit();
					if (!m_isLMBClicked)
					{
						m_isLMBClicked = true;
						m_context->m_elementInMouseFocus = this;
					}
				}
				if (m_input->m_isRMBDown)
				{
					OnRMBHit();
					if (!m_isRMBClicked)
					{
						m_isRMBClicked = true;
						m_context->m_elementInMouseFocus = this;
					}
				}
				if (m_input->m_isMMBDown)
				{
					OnMMBHit();
					if (!m_isMMBClicked)
					{
						m_isMMBClicked = true;
						m_context->m_elementInMouseFocus = this;
					}
				}
			}
			m_context->m_elementUnderCursor = this;
		}
	}
}

void alGUIElement::Rebuild()
{
	m_buildArea.x = m_position.x;
	m_buildArea.y = m_position.y;
	m_buildArea.z = m_buildArea.x + m_size.x;
	m_buildArea.w = m_buildArea.y + m_size.y;
	if (m_parent)
	{
		m_buildArea.x += m_parent->m_buildArea.x;
		m_buildArea.y += m_parent->m_buildArea.y;
		m_buildArea.z += m_parent->m_buildArea.x;
		m_buildArea.w += m_parent->m_buildArea.y;
		/*float32_t parentRectSizeX_1 = 1.f / (m_parent->m_buildArea.z - m_parent->m_buildArea.x);
		float32_t parentRectSizeY_1 = 1.f / (m_parent->m_buildArea.w - m_parent->m_buildArea.y);

		float32_t parentCreationCenter_X = m_parent->m_buildAreaOnCreation.x +
			((m_parent->m_buildAreaOnCreation.z - m_parent->m_buildAreaOnCreation.x) * 0.5f);
		float32_t parentCreationCenter_Y = m_parent->m_buildAreaOnCreation.y +
			((m_parent->m_buildAreaOnCreation.w - m_parent->m_buildAreaOnCreation.y) * 0.5f);

		float32_t parentCurrentCenter_X = m_parent->m_buildArea.x +
			((m_parent->m_buildArea.z - m_parent->m_buildArea.x) * 0.5f);
		float32_t parentCurrentCenter_Y = m_parent->m_buildArea.y +
			((m_parent->m_buildArea.w - m_parent->m_buildArea.y) * 0.5f);

		float32_t parentRectSizeDiff_X = parentCurrentCenter_X - parentCreationCenter_X;
		float32_t parentRectSizeDiff_Y = parentCurrentCenter_Y - parentCreationCenter_Y;*/
		switch (m_alignment)
		{
		default:
		case alGUIElementAlignment::LeftTop:
			break;
		case alGUIElementAlignment::RightTop:
		{
			m_buildArea.x = m_parent->m_buildArea.z - m_position.x - m_size.x;
			m_buildArea.y = m_parent->m_buildArea.y + m_position.y;
			m_buildArea.z = m_buildArea.x + m_size.x;
			m_buildArea.w = m_buildArea.y + m_size.y;
		}
		//	m_buildArea.x = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.x);
		//	m_buildArea.z = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.z);
			break;
		case alGUIElementAlignment::LeftBottom:
			break;
		case alGUIElementAlignment::RightBottom:
			break;
		case alGUIElementAlignment::Center:
			break;
		case alGUIElementAlignment::Top:
			break;
		case alGUIElementAlignment::Left:
			break;
		case alGUIElementAlignment::Right:
			break;
		case alGUIElementAlignment::Bottom:
			break;
		}
	}

	/*m_buildArea.x = m_position.x;
	m_buildArea.y = m_position.y;
	m_buildArea.z = m_buildArea.x + m_size.x;
	m_buildArea.w = m_buildArea.y + m_size.y;*/

	/*if (m_parent)
	{
		m_buildArea.x += m_parent->m_buildArea.x;
		m_buildArea.y += m_parent->m_buildArea.y;
		m_buildArea.z = m_buildArea.x + m_size.x;
		m_buildArea.w = m_buildArea.y + m_size.y;
	}*/

	m_sensorArea = m_buildArea;
	m_sensorArea += m_sensorAreaIndent;

	m_clipArea = m_buildArea;
	m_clipArea += m_clipAreaIndent;
	if (m_parent)
	{
		if (m_parent->m_clipArea.x > m_clipArea.x)
			m_clipArea.x = m_parent->m_clipArea.x;
		if (m_parent->m_clipArea.y > m_clipArea.y)
			m_clipArea.y = m_parent->m_clipArea.y;
		if (m_parent->m_clipArea.z < m_clipArea.z)
			m_clipArea.z = m_parent->m_clipArea.z;
		if (m_parent->m_clipArea.w < m_clipArea.w)
			m_clipArea.w = m_parent->m_clipArea.w;
	}

	if (m_clipArea.x > m_sensorArea.x)
		m_sensorArea.x = m_clipArea.x;

	if (m_clipArea.z < m_sensorArea.z)
		m_sensorArea.z = m_clipArea.z;

	if (m_clipArea.y > m_sensorArea.y)
		m_sensorArea.y = m_clipArea.y;

	if (m_clipArea.w < m_sensorArea.w)
		m_sensorArea.w = m_clipArea.w;

	if (m_sensorArea.x < 0.f)
		m_sensorArea.x = 0.f;
	if (m_sensorArea.y < 0.f)
		m_sensorArea.y = 0.f;

	if (m_sensorArea.z > m_context->GetWindow()->m_clientSize.x)
		m_sensorArea.z = m_context->GetWindow()->m_clientSize.x;
	if (m_sensorArea.w > m_context->GetWindow()->m_clientSize.y)
		m_sensorArea.w = m_context->GetWindow()->m_clientSize.y;

}
