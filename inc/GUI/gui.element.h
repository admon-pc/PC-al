#ifndef _AL_GUIELEMENTH_
#define _AL_GUIELEMENTH_

#include "Input/alInput.h"

enum class alGUIElementAlignment : uint32_t
{
	LeftTop,
	RightTop,
	LeftBottom,
	RightBottom,
	Center,
	Top,
	Left,
	Right,
	Bottom,
};

class alGUIElement : public alUserData, public alID
{
protected:
	alGS* m_gs = 0;
	alInput* m_input = 0;
public:
	alGUIElement(alGUIContext* ctx, const alVec2f& position, const alVec2f& size);
	virtual ~alGUIElement() {}

	alGUIColorTheme* m_colorTheme = 0;

	virtual void Draw(float32_t dt) = 0;
	
	virtual void OnDraw() {}
	virtual void OnMouseEnter() {}
	virtual void OnMouseMove() {}
	virtual void OnMouseLeave() {}
	virtual void OnLMBHit() {}
	virtual void OnRMBHit() {}
	virtual void OnMMBHit() {}
	virtual void OnLMBRelease() {}
	virtual void OnRMBRelease() {}
	virtual void OnMMBRelease() {}

	virtual void Update(float32_t dt);

	virtual float32_t GetHeight()
	{
		return m_buildArea.w - m_buildArea.y;
	}

	alGUIContext* m_context = nullptr;
	alList<alGUIElement*> m_children;

	virtual void SetVisible(bool set)
	{
		m_visible = set;
	}

	virtual alGUIElement* GetParent() { return m_parent; }
	virtual void SetParent(alGUIElement* parent)
	{
		if (m_parent)
		{
			m_parent->m_children.erase_first(this);
		}

		m_parent = parent;

		if (parent)
			parent->m_children.push_back(this);
	}


	virtual void UpdateTransform()
	{
		if (!m_visible)
			return;

	/*	if (m_onUpdateTransform)
		{
			m_onUpdateTransform(this);
			return;
		}*/

	//	m_offsetTransformed = m_offset;
		//if (m_parent)
		//{

		////	m_offsetTransformed += m_parent->m_offsetTransformed;

		//	m_buildArea.x += m_parent->m_buildArea.x;
		//	m_buildArea.y += m_parent->m_buildArea.y;
		//	m_buildArea.z += m_parent->m_buildArea.x;
		//	m_buildArea.w += m_parent->m_buildArea.y;
		//	m_sensorArea.x += m_parent->m_sensorArea.x;
		//	m_sensorArea.y += m_parent->m_sensorArea.y;
		//	m_sensorArea.z += m_parent->m_sensorArea.x;
		//	m_sensorArea.w += m_parent->m_sensorArea.y;
		//	m_clipArea.x += m_parent->m_clipArea.x;
		//	m_clipArea.y += m_parent->m_clipArea.y;
		//	m_clipArea.z += m_parent->m_clipArea.x;
		//	m_clipArea.w += m_parent->m_clipArea.y;


		//	float32_t parentRectSizeX_1 = 1.f / (m_parent->m_buildArea.z - m_parent->m_buildArea.x);
		//	float32_t parentRectSizeY_1 = 1.f / (m_parent->m_buildArea.w - m_parent->m_buildArea.y);

		//	float32_t parentCreationCenter_X = m_parent->m_buildAreaOnCreation.x +
		//		((m_parent->m_buildAreaOnCreation.z - m_parent->m_buildAreaOnCreation.x) * 0.5f);
		//	float32_t parentCreationCenter_Y = m_parent->m_buildAreaOnCreation.y +
		//		((m_parent->m_buildAreaOnCreation.w - m_parent->m_buildAreaOnCreation.y) * 0.5f);

		//	float32_t parentCurrentCenter_X = m_parent->m_buildArea.x +
		//		((m_parent->m_buildArea.z - m_parent->m_buildArea.x) * 0.5f);
		//	float32_t parentCurrentCenter_Y = m_parent->m_buildArea.y +
		//		((m_parent->m_buildArea.w - m_parent->m_buildArea.y) * 0.5f);

		//	float32_t parentRectSizeDiff_X = parentCurrentCenter_X - parentCreationCenter_X;
		//	float32_t parentRectSizeDiff_Y = parentCurrentCenter_Y - parentCreationCenter_Y;



		//	switch (m_alignment)
		//	{
		//	case alGUIElementAlignment::LeftTop:
		//		break;
		//	case alGUIElementAlignment::RightTop:
		//		m_buildArea.x = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.x);
		//		m_buildArea.z = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.z);

		//		m_sensorArea.x = m_buildArea.x;
		//		m_sensorArea.z = m_buildArea.z;

		//		m_clipArea.x = m_buildArea.x;
		//		m_clipArea.z = m_buildArea.z;
		//		break;
		//	case alGUIElementAlignment::LeftBottom:
		//		m_buildArea.y = m_parent->m_buildArea.w - (m_parent->m_buildAreaOnCreation.w - m_buildAreaOnCreation.y);
		//		m_buildArea.w = m_parent->m_buildArea.w - (m_parent->m_buildAreaOnCreation.w - m_buildAreaOnCreation.w);

		//		m_sensorArea.y = m_buildArea.y;
		//		m_sensorArea.w = m_buildArea.w;

		//		m_clipArea.y = m_buildArea.y;
		//		m_clipArea.w = m_buildArea.w;
		//		break;
		//	case alGUIElementAlignment::RightBottom:
		//		m_buildArea.x = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.x);
		//		m_buildArea.y = m_parent->m_buildArea.w - (m_parent->m_buildAreaOnCreation.w - m_buildAreaOnCreation.y);
		//		m_buildArea.z = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.z);
		//		m_buildArea.w = m_parent->m_buildArea.w - (m_parent->m_buildAreaOnCreation.w - m_buildAreaOnCreation.w);

		//		m_sensorArea.x = m_buildArea.x;
		//		m_sensorArea.y = m_buildArea.y;
		//		m_sensorArea.z = m_buildArea.z;
		//		m_sensorArea.w = m_buildArea.w;

		//		m_clipArea.x = m_buildArea.x;
		//		m_clipArea.y = m_buildArea.y;
		//		m_clipArea.z = m_buildArea.z;
		//		m_clipArea.w = m_buildArea.w;
		//		break;
		//	case alGUIElementAlignment::Top:
		//		m_buildArea.x = m_buildAreaOnCreation.x + parentRectSizeDiff_X;
		//		m_buildArea.z = m_buildAreaOnCreation.z + parentRectSizeDiff_X;

		//		m_sensorArea.x = m_buildArea.x;
		//		m_sensorArea.z = m_buildArea.z;

		//		m_clipArea.x = m_buildArea.x;
		//		m_clipArea.z = m_buildArea.z;
		//		break;
		//	case alGUIElementAlignment::Left:
		//		m_buildArea.y = m_buildAreaOnCreation.y + parentRectSizeDiff_Y;
		//		m_buildArea.w = m_buildAreaOnCreation.w + parentRectSizeDiff_Y;

		//		m_sensorArea.y = m_buildArea.y;
		//		m_sensorArea.w = m_buildArea.w;

		//		m_clipArea.y = m_buildArea.y;
		//		m_clipArea.w = m_buildArea.w;
		//		break;
		//	case alGUIElementAlignment::Right:
		//		m_buildArea.y = m_buildAreaOnCreation.y + parentRectSizeDiff_Y;
		//		m_buildArea.w = m_buildAreaOnCreation.w + parentRectSizeDiff_Y;

		//		m_sensorArea.y = m_buildArea.y;
		//		m_sensorArea.w = m_buildArea.w;

		//		m_clipArea.y = m_buildArea.y;
		//		m_clipArea.w = m_buildArea.w;

		//		// + rightTop
		//		m_buildArea.x = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.x);
		//		m_buildArea.z = m_parent->m_buildArea.z - (m_parent->m_buildAreaOnCreation.z - m_buildAreaOnCreation.z);

		//		m_sensorArea.x = m_buildArea.x;
		//		m_sensorArea.z = m_buildArea.z;

		//		m_clipArea.x = m_buildArea.x;
		//		m_clipArea.z = m_buildArea.z;
		//		break;
		//	case alGUIElementAlignment::Bottom:
		//		m_buildArea.x = m_buildAreaOnCreation.x + parentRectSizeDiff_X;
		//		m_buildArea.z = m_buildAreaOnCreation.z + parentRectSizeDiff_X;

		//		m_sensorArea.x = m_buildArea.x;
		//		m_sensorArea.z = m_buildArea.z;

		//		m_clipArea.x = m_buildArea.x;
		//		m_clipArea.z = m_buildArea.z;

		//		// + leftBottom
		//		m_buildArea.y = m_parent->m_buildArea.w - (m_parent->m_buildAreaOnCreation.w - m_buildAreaOnCreation.y);
		//		m_buildArea.w = m_parent->m_buildArea.w - (m_parent->m_buildAreaOnCreation.w - m_buildAreaOnCreation.w);

		//		m_sensorArea.y = m_buildArea.y;
		//		m_sensorArea.w = m_buildArea.w;

		//		m_clipArea.y = m_buildArea.y;
		//		m_clipArea.w = m_buildArea.w;

		//		break;
		//	case alGUIElementAlignment::Center:
		//	default:
		//		// Right
		//		m_buildArea.y = m_buildAreaOnCreation.y + parentRectSizeDiff_Y;
		//		m_buildArea.w = m_buildAreaOnCreation.w + parentRectSizeDiff_Y;

		//		m_sensorArea.y = m_buildArea.y;
		//		m_sensorArea.w = m_buildArea.w;

		//		m_clipArea.y = m_buildArea.y;
		//		m_clipArea.w = m_buildArea.w;

		//		// bottom
		//		m_buildArea.x = m_buildAreaOnCreation.x + parentRectSizeDiff_X;
		//		m_buildArea.z = m_buildAreaOnCreation.z + parentRectSizeDiff_X;

		//		m_sensorArea.x = m_buildArea.x;
		//		m_sensorArea.z = m_buildArea.z;

		//		m_clipArea.x = m_buildArea.x;
		//		m_clipArea.z = m_buildArea.z;
		//		break;
		//	}
		//}
	}

	// All elements are placed in 2D space, and for this I need
	// two things. 1 - position, 2 - size.
	// Using position and size I can calculate ...Area vectors
	// It will be calculated in Rebuild method
	alVec2f m_position;
	alVec2f m_size;

	alVec4f m_buildArea;  // using for create triangles
	alVec4f m_sensorArea; //   for detecting cursor in area or not
	alVec4f m_clipArea;   //   for scissor
	
	// It will be set by panel
	// It will be used in aligment calculation
	void SetPositionAndSize(float32_t pX, float32_t pY, float32_t sX, float32_t sY)
	{
		m_position.x = pX;
		m_position.y = pY;
		m_size.x = sX;
		m_size.y = sY;
	}

	// for some reason you can change sensor area
	alVec4f m_sensorAreaIndent;
	alVec4f m_clipAreaIndent;
	
	bool m_isCursorInSensorArea = false;
	bool m_isLMBClicked = false;
	bool m_isRMBClicked = false;
	bool m_isMMBClicked = false;
	bool m_isX1MBClicked = false;
	bool m_isX2MBClicked = false;

	virtual void Rebuild();



	// Must calculate using parents _Transformed vectors.
	//  build elements and detect cursor in area only using this.
	//alVec4f m_buildArea;
	//alVec4f m_sensorArea;
	//alVec4f m_clipArea;

	//alVec2f m_offset;
	//alVec2f m_offsetTransformed;

	alGUIElementAlignment m_alignment = alGUIElementAlignment::LeftTop;


	// use SetVisible();
	bool m_visible = true;

	bool m_ignoreInput = false; // if true then only draw

	// For exmple for text in button. Buttons will draw elements in button->Draw();
	bool m_ignoreDraw = false;

	//bool m_isInActiveAreaRect = false;

	// it should be root object by default. 
	// it always must be if(m_parent) so this if unnecessary
	alGUIElement* m_parent = nullptr;
};

#endif

