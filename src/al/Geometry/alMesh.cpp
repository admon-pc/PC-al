#include "al.h"
#include "Geometry/alMesh.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alMesh::alMesh()
{
}

alMesh::~alMesh()
{
	if (m_vertices) 
		alMemory::Free(m_vertices);
	if (m_indices) 
		alMemory::Free(m_indices);
}

// Tangents from IrrLicht
/*
* The Irrlicht Engine License
  ===========================

  Copyright (C) 2002-2015 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgement in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be clearly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
void alMesh::CalculateTangents(alVec3f& normal, alVec3f& tangent, alVec3f& binormal,
	const alVec3f& vt1, const alVec3f& vt2, const alVec3f& vt3, // vertices
	const alVec2f& tc1, const alVec2f& tc2, const alVec2f& tc3) // texture coords
{
	alVec3f e1 = vt2 - vt1;
	alVec3f e2 = vt3 - vt1;
	normal = e1.Cross(e2);
	normal.Normalize2();
	float32_t deltaX1 = tc1.x - tc2.x;
	float32_t deltaX2 = tc3.x - tc1.x;
	binormal = (e1 * deltaX2) - (e2 * deltaX1);
	binormal.Normalize2();
	float32_t deltaY1 = tc1.y - tc2.y;
	float32_t deltaY2 = tc3.y - tc1.y;
	tangent = (e1 * deltaY2) - (e2 * deltaY1);
	tangent.Normalize2();
	alVec3f txb = tangent.Cross(binormal);
	if (txb.Dot(normal) < 0.0f)
	{
		tangent *= -1.0f;
		binormal *= -1.0f;
	}
}


void alMesh::GenerateTangents_u16()
{
	uint16_t* idx = (uint16_t*)m_indices;
	alVertexTriangle* v = (alVertexTriangle*)m_vertices;

	alVec3f localNormal;
	for (uint32_t i = 0; i < m_iCount; i += 3)
	{
		CalculateTangents(
			localNormal,
			v[idx[i + 0]].Tangent,
			v[idx[i + 0]].Binormal,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].UV1,
			v[idx[i + 1]].UV1,
			v[idx[i + 2]].UV1);
		//if (recalculateNormals)
		//v[idx[i+0]].Normal=localNormal;

		CalculateTangents(
			localNormal,
			v[idx[i + 1]].Tangent,
			v[idx[i + 1]].Binormal,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].UV1,
			v[idx[i + 2]].UV1,
			v[idx[i + 0]].UV1);
		//if (recalculateNormals)
		//v[idx[i+1]].Normal=localNormal;

		CalculateTangents(
			localNormal,
			v[idx[i + 2]].Tangent,
			v[idx[i + 2]].Binormal,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].UV1,
			v[idx[i + 0]].UV1,
			v[idx[i + 1]].UV1);
		//if (recalculateNormals)
		//v[idx[i+2]].Normal=localNormal;
	}
}

void alMesh::GenerateTangents_u32()
{
	uint32_t* idx = (uint32_t*)m_indices;
	alVertexTriangle* v = (alVertexTriangle*)m_vertices;

	alVec3f localNormal;
	for (uint32_t i = 0; i < m_iCount; i += 3)
	{
		CalculateTangents(
			localNormal,
			v[idx[i + 0]].Tangent,
			v[idx[i + 0]].Binormal,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].UV1,
			v[idx[i + 1]].UV1,
			v[idx[i + 2]].UV1);
		//if (recalculateNormals)
		//v[idx[i+0]].Normal=localNormal;

		CalculateTangents(
			localNormal,
			v[idx[i + 1]].Tangent,
			v[idx[i + 1]].Binormal,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].UV1,
			v[idx[i + 2]].UV1,
			v[idx[i + 0]].UV1);
		//if (recalculateNormals)
		//v[idx[i+1]].Normal=localNormal;

		CalculateTangents(
			localNormal,
			v[idx[i + 2]].Tangent,
			v[idx[i + 2]].Binormal,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].UV1,
			v[idx[i + 0]].UV1,
			v[idx[i + 1]].UV1);
		//if (recalculateNormals)
		//v[idx[i+2]].Normal=localNormal;
	}
}

void alMesh::GenerateTangents()
{
	if (m_vertexType != alMeshVertexType::Triangle)
		return;

	switch (m_indexType)
	{
	case alMeshIndexType::u16:
	default:
		GenerateTangents_u16();
		break;
	case alMeshIndexType::u32:
		GenerateTangents_u32();
		break;
	}
}

void alMesh::PrintAllInfo(bool onlyPosition)
{
	alLog::PrintInfo("\n\nalMesh\n");
	alLog::Print("Name: ");
	if (m_name.size())
	{
		alLog::Print("%s", m_name.c_str());
	}
	alLog::Print("\n");

	uint16_t* ind16 = 0;
	uint32_t* ind32 = 0;

	alLog::Print("Index type: ");
	switch (m_indexType)
	{
	default:
	case alMeshIndexType::u16:
		ind16 = (uint16_t*)m_indices;
		alLog::Print("u16\n");
		break;
	case alMeshIndexType::u32:
		ind32 = (uint32_t*)m_indices;
		alLog::Print("u16\n");
		break;
	}

	alVertexGUI* vGUI = 0;
	alVertexTriangle* vTriangle = 0;
	alVertexAnimatedTriangle* vAnimatedTriangle = 0;
	alVertexAnimatedLine* vAnimatedLine = 0;
	alVertexLine* vLine = 0;
	alVertexPoint* vPoint = 0;
	alVertexAnimatedPoint* vAnimatedPoint = 0;
	alLog::Print("Vertex type: ");
	switch (m_vertexType)
	{
	default:
	case alMeshVertexType::Null:
		alLog::Print("Null\n");
		break;
	case alMeshVertexType::GUI:
		vGUI = (alVertexGUI*)m_vertices;
		alLog::Print("GUI\n");
		break;
	case alMeshVertexType::Triangle:
		vTriangle = (alVertexTriangle*)m_vertices;
		alLog::Print("Triangle\n");
		break;
	case alMeshVertexType::AnimatedTriangle:
		vAnimatedTriangle = (alVertexAnimatedTriangle*)m_vertices;
		alLog::Print("AnimatedTriangle\n");
		break;
	case alMeshVertexType::Line:
		vLine = (alVertexLine*)m_vertices;
		alLog::Print("Line\n");
		break;
	case alMeshVertexType::AnimatedLine:
		vAnimatedLine = (alVertexAnimatedLine*)m_vertices;
		alLog::Print("AnimatedLine\n");
		break;
	case alMeshVertexType::Point:
		vPoint = (alVertexPoint*)m_vertices;
		alLog::Print("Point\n");
		break;
	case alMeshVertexType::AnimatedPoint:
		vAnimatedPoint = (alVertexAnimatedPoint*)m_vertices;
		alLog::Print("AnimatedPoint\n");
		break;
	}

	alLog::Print("IndNum: %u\n", m_iCount);
	alLog::Print("VertNum: %u\n", m_vCount);
	alLog::Print("Stride: %u\n", m_stride);
	alLog::Print("AABB min: [%f][%f][%f]\n", m_aabb.m_min.x, m_aabb.m_min.y, m_aabb.m_min.z);
	alLog::Print("AABB max: [%f][%f][%f]\n", m_aabb.m_max.x, m_aabb.m_max.y, m_aabb.m_max.z);
	alLog::Print("Indices:\n\t\t");


	for (uint32_t i = 0; i < m_iCount; ++i)
	{
		switch (m_indexType)
		{
		default:
		case alMeshIndexType::u16:
			alLog::Print("%i ", *ind16);
			++ind16;
			break;
		case alMeshIndexType::u32:
			alLog::Print("%i ", *ind32);
			++ind32;
			break;
		}
	}


	alLog::Print("\n");
	alLog::Print("Vertices:\n");
	for (uint32_t i = 0; i < m_vCount; ++i)
	{
		switch (m_vertexType)
		{
		default:
		case alMeshVertexType::Null:
			break;
		case alMeshVertexType::GUI:
		{
			auto ptr = vGUI; ++vGUI;
			alLog::Print("#%i P[%f][%f]\n", i, ptr->Position.x, ptr->Position.y);
			if (onlyPosition)
				break;
			alLog::Print("#%i U[%f][%f]\n", i, ptr->UV.x, ptr->UV.y);
		}break;
		case alMeshVertexType::Triangle:
		{
			auto ptr = vTriangle; ++vTriangle;
			alLog::Print("#%i P[%f][%f][%f]\n", i, ptr->Position.x, ptr->Position.y, ptr->Position.z);
			if (onlyPosition)
				break;
			alLog::Print("#%i N[%f][%f][%f]\n", i, ptr->Normal.x, ptr->Normal.y, ptr->Normal.z);
			alLog::Print("#%i C[%f][%f][%f]\n", i, ptr->Color.x, ptr->Color.y, ptr->Color.z);
			alLog::Print("#%i B[%f][%f][%f]\n", i, ptr->Binormal.x, ptr->Binormal.y, ptr->Binormal.z);
			alLog::Print("#%i T[%f][%f][%f]\n", i, ptr->Tangent.x, ptr->Tangent.y, ptr->Tangent.z);
			alLog::Print("#%i U1[%f][%f][%f]\n", i, ptr->UV1.x, ptr->UV1.y);
			alLog::Print("#%i U2[%f][%f][%f]\n", i, ptr->UV2.x, ptr->UV2.y);
		}break;
		case alMeshVertexType::AnimatedTriangle:
		{
			auto ptr = vAnimatedTriangle; ++vAnimatedTriangle;
			alLog::Print("#%i P[%f][%f][%f]\n", i, ptr->Position.x, ptr->Position.y, ptr->Position.z);
			if (onlyPosition)
				break;
			alLog::Print("#%i N[%f][%f][%f]\n", i, ptr->Normal.x, ptr->Normal.y, ptr->Normal.z);
			alLog::Print("#%i C[%f][%f][%f]\n", i, ptr->Color.x, ptr->Color.y, ptr->Color.z);
			alLog::Print("#%i B[%f][%f][%f]\n", i, ptr->Binormal.x, ptr->Binormal.y, ptr->Binormal.z);
			alLog::Print("#%i T[%f][%f][%f]\n", i, ptr->Tangent.x, ptr->Tangent.y, ptr->Tangent.z);
			alLog::Print("#%i U1[%f][%f][%f]\n", i, ptr->UV1.x, ptr->UV1.y);
			alLog::Print("#%i U2[%f][%f][%f]\n", i, ptr->UV2.x, ptr->UV2.y);
			alLog::Print("#%i I[%u][%u][%u][%u]\n", i, ptr->Bones.x, ptr->Bones.y, ptr->Bones.z, ptr->Bones.w);
			alLog::Print("#%i W[%f][%f][%f][%f]\n", i, ptr->Weights.x, ptr->Weights.y, ptr->Weights.z, ptr->Weights.w);
		}break;
		case alMeshVertexType::Line:
		{
			auto ptr = vLine; ++vLine;
			alLog::Print("#%i P[%f][%f][%f]\n", i, ptr->Position.x, ptr->Position.y, ptr->Position.z);
			if (onlyPosition)
				break;
			alLog::Print("#%i N[%f][%f][%f]\n", i, ptr->Normal.x, ptr->Normal.y, ptr->Normal.z);
			alLog::Print("#%i C[%f][%f][%f]\n", i, ptr->Color.x, ptr->Color.y, ptr->Color.z);
		}break;
		case alMeshVertexType::AnimatedLine:
		{
			auto ptr = vAnimatedLine; ++vAnimatedLine;
			alLog::Print("#%i P[%f][%f][%f]\n", i, ptr->Position.x, ptr->Position.y, ptr->Position.z);
			if (onlyPosition)
				break;
			alLog::Print("#%i N[%f][%f][%f]\n", i, ptr->Normal.x, ptr->Normal.y, ptr->Normal.z);
			alLog::Print("#%i C[%f][%f][%f]\n", i, ptr->Color.x, ptr->Color.y, ptr->Color.z);
			alLog::Print("#%i I[%u][%u][%u][%u]\n", i, ptr->Bones.x, ptr->Bones.y, ptr->Bones.z, ptr->Bones.w);
			alLog::Print("#%i W[%f][%f][%f][%f]\n", i, ptr->Weights.x, ptr->Weights.y, ptr->Weights.z, ptr->Weights.w);
		}break;
		case alMeshVertexType::Point:
		{
			auto ptr = vPoint; ++vPoint;
			alLog::Print("#%i P[%f][%f][%f]\n", i, ptr->Position.x, ptr->Position.y, ptr->Position.z);
			if (onlyPosition)
				break;
			alLog::Print("#%i C[%f][%f][%f]\n", i, ptr->Color.x, ptr->Color.y, ptr->Color.z);
		}break;
		case alMeshVertexType::AnimatedPoint:
		{
			auto ptr = vAnimatedPoint; ++vAnimatedPoint;

			alLog::Print("#%i P[%f][%f][%f]\n", i, ptr->Position.x, ptr->Position.y, ptr->Position.z);
			if (onlyPosition)
				break;
			alLog::Print("#%i C[%f][%f][%f]\n", i, ptr->Color.x, ptr->Color.y, ptr->Color.z);
			alLog::Print("#%i I[%u][%u][%u][%u]\n", i, ptr->Bones.x, ptr->Bones.y, ptr->Bones.z, ptr->Bones.w);
			alLog::Print("#%i W[%f][%f][%f][%f]\n", i, ptr->Weights.x, ptr->Weights.y, ptr->Weights.z, ptr->Weights.w);
		}break;
		}
	}
	alLog::Print("\nEnd\n");
}
