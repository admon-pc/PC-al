#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGSTexture.h"
#include "GS/alGS.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIContext::alGUIContext(alSystemWindow* w, alGS* gs)
	:
	m_window(w),
	m_gs(gs)
{
}

alGUIContext::~alGUIContext()
{
	DeleteAllPanels();
}

void alGUIContext::Update(float32_t dt)
{
	m_cursorType = alCursorType::Arrow;
	m_isElementUnderCursor = false;
	//m_elementUnderCursor = 0;
	//m_panelUnderCursor = 0;
	if (m_activePopup)
	{
		m_activePopup->Update(dt);
	}
	else
	{
		if (m_activeElement)
		{
			m_activeElement->Update(dt);
		}
		else
		{
			for (size_t i = 0; i < m_panels.m_size; ++i)
			{
				m_panels.m_data[i]->Update(dt);
			}
		}
	}

	if (m_elementUnderCursor && !m_isElementUnderCursor)
	{
		m_elementUnderCursor->OnMouseLeave();
		m_elementUnderCursor = 0;
	}
}

void alGUIContext::Draw(float32_t dt)
{
	for (size_t i = 0; i < m_panels.m_size; ++i)
	{
		if (m_activeElement == m_panels.m_data[i])
			continue;

		m_panels.m_data[i]->Draw(dt);
	}

	if (m_activeElement)
	{
		m_activeElement->Draw(dt);
	}

	if (m_activePopup)
	{
		m_activePopup->Draw(dt);
	}
}


alGUIPanel* alGUIContext::GetNewPanel()
{
	auto c = alCreate<alGUIPanel>(this);
	m_panels.push_back(c);
	return c;
}

void alGUIContext::DeleteAllPanels()
{
	for (size_t i = 0; i < m_panels.m_size; ++i)
	{
		alDestroy(m_panels.m_data[i]);
	}
	m_panels.clear();

	m_elementUnderCursor = 0;
}

alGSMesh* alGUIContext::_create_bg_mesh(
	const alVec4f& buildRect,
	float32_t smooth_corner_indent,
	uint32_t smooth_corner_iterations,
	alVec4f* UV)
{
	alVec2f buildRectCenter;
	buildRectCenter.x = (buildRect.z - buildRect.x) * 0.5f;
	buildRectCenter.y = (buildRect.w - buildRect.y) * 0.5f;

	auto _generate_corner = [&](
		alArray<alVertexGUI>& arr,
		float32_t base_angle,
		uint32_t sciterations,
		float32_t roundSize,
		const alVec2f& pivot		
		)
	{
		uint32_t i_sz = sciterations + 2;
		float32_t a_step = 90.f / (float32_t)(i_sz - 1);
		float32_t angle = base_angle;

		alVec2f base_pos(roundSize, 0.f);

		for (uint32_t i = 0; i < i_sz; ++i)
		{
			auto cs = std::cos(alMath::DegToRad(angle));
			auto sn = -std::sin(alMath::DegToRad(angle));

			alVec2f position;

			position.x = base_pos.x * cs - base_pos.y * sn;
			position.y = base_pos.x * sn + base_pos.y * cs;

			angle += a_step;

			alVertexGUI v;
			v.Position = position + pivot;
			arr.push_back(v);
		}
	};

	alArray<alVertexGUI> vertices;

	uint32_t _a = smooth_corner_iterations;
	float32_t indent = smooth_corner_indent;

	alVec2f lt, rb;
	rb.Set(1.f, 1.f);
	if (UV)
	{
		lt.Set(UV->x, UV->y);
		rb.Set(UV->z, UV->w);
	}

	_generate_corner(
		vertices, 
		0.f, 
		smooth_corner_iterations,
		smooth_corner_indent, 
		alVec2f(buildRect.z - indent, buildRect.y + indent)
		);

	_generate_corner(
		vertices,
		90.f,
		smooth_corner_iterations,
		smooth_corner_indent,
		alVec2f(buildRect.x + indent, buildRect.y + indent));

	_generate_corner(
		vertices,
		180.f,
		smooth_corner_iterations,
		smooth_corner_indent,
		alVec2f(buildRect.x + indent, buildRect.w - indent));

	_generate_corner(
		vertices,
		270.f,
		smooth_corner_iterations,
		smooth_corner_indent,
		alVec2f(buildRect.z - indent, buildRect.w - indent));

	auto mesh = alCreate<alMesh>();

	mesh->m_iCount = (vertices.m_size - 2) * 3;
	mesh->m_vCount = vertices.m_size;
	mesh->m_stride = sizeof(alVertexGUI);
	mesh->m_vertexType = alMeshVertexType::GUI;
	mesh->m_vertices = (uint8_t*)alMemory::Malloc(mesh->m_vCount * mesh->m_stride);
	mesh->m_indices = (uint8_t*)alMemory::Malloc(mesh->m_iCount * sizeof(uint16_t));
	uint16_t* inds = (uint16_t*)mesh->m_indices;

	alVertexGUI* vertex = (alVertexGUI*)mesh->m_vertices;
	for (uint32_t i = 0; i < vertices.m_size; ++i)
	{
		vertex->Position = vertices.m_data[i].Position;
		vertex->UV = vertices.m_data[i].UV;
		vertex++;
	}

	uint32_t triNum = vertices.m_size - 2;
	for (uint32_t i = 0; i < triNum; ++i)
	{
		*inds = 0;
		inds++;
		*inds = i + 2;
		inds++;
		*inds = i + 1;
		inds++;
	}

	alGSMeshInfo info;
	info.m_meshPtr = mesh;

	auto meshGPU = m_gs->CreateMesh(&info);
	alDestroy(mesh);
	return meshGPU;
}





























