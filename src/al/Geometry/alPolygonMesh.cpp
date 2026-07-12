#include "al.h"
#include "Geometry/alTriangle.h"
#include "Geometry/alPopygonMesh.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alPolygon::alPolygon()
{
}

alPolygon::~alPolygon()
{
}

void alPolygon::CopyData(alPolygon* other)
{
}

alListNode<alPolygon::_vertex_data>* alPolygon::FindVertex(alVertex* v)
{
	auto curV = m_verts.m_head;
	auto lastV = curV->m_left;
	while (true)
	{
		if (curV->m_data.m_vertex == v)
			break;
		if (curV == lastV)
			break;
		curV = curV->m_right;
	}
	return curV;
}

// !!!!
// CHECK ORDER
bool alPolygon::IsVisible()
{
	// HERE
	auto vertex_1 = m_verts.m_head;
	auto vertex_3 = vertex_1->m_right;
	auto vertex_2 = vertex_3->m_right;
	while (true)
	{
		auto a = vertex_2->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
		auto b = vertex_3->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;

		alVec3f n;
		a.Cross2(b, n);

		float32_t area = 0.5f * sqrt(n.Dot());

		if (area > 0.001)
			return true;

		vertex_2 = vertex_2->m_right;
		vertex_3 = vertex_3->m_right;
		if (vertex_3 == vertex_1)
			break;
	}
	return false;
}


// !!!!
// CHECK ORDER
void alPolygon::CalculateNormal()
{
	auto n = GetFaceNormalCalculateNew();

	{
		auto vertex_1 = m_verts.m_head;
		vertex_1->m_data.m_normal = n;
		// HERE
		auto vertex_3 = vertex_1->m_right;
		auto vertex_2 = vertex_3->m_right;
		while (true) 
		{
			vertex_2->m_data.m_normal = n;
			vertex_3->m_data.m_normal = n;
			// ===============================
			vertex_2 = vertex_2->m_right;
			vertex_3 = vertex_3->m_right;

			if (vertex_2 == vertex_1)
				break;
		}
	}
}

alVec3f alPolygon::GetFaceNormal()
{
	return m_faceNormal;
}

alVec3f alPolygon::GetFaceNormalCalculateNew()
{
	alVec3f _n;
	auto vertex_1 = m_verts.m_head;
	auto vertex_2 = vertex_1->m_right;
	auto vertex_3 = vertex_2->m_right;

	auto e1 = vertex_2->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
	auto e2 = vertex_3->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
	auto n = e1.Cross(e2);
	n.Normalize2();
	
	m_faceNormal = n;
	return n;
}

void alPolygon::Flip()
{
	m_verts.reverse();
}

void alPolygon::FixOrder(float32_t lineLineCollisionLen)
{
	// only if 4 or more vertices
	uint32_t num = 0;
	auto curV = m_verts.m_head;
	auto lastV = curV->m_left;
	while (true)
	{
		++num;
		if (curV == lastV)
			break;
		curV = curV->m_right;
	}
	if (num < 4)
		return;

	for (uint32_t i = 0; i < num * 4; ++i)
	{
		curV = m_verts.m_head;
		lastV = curV->m_left;
		while (true)
		{
			alPair<alListNode<_vertex_data>*, alListNode<_vertex_data>*> current;
			alPair<alListNode<_vertex_data>*, alListNode<_vertex_data>*> prev;

			current.m_first = curV;
			current.m_second = current.m_first->m_right;

			prev.m_second = current.m_first;
			prev.m_first = prev.m_second->m_left;

			auto tgCurr = current.m_second->m_right;
			auto tgLast = tgCurr->m_left;
			while (true)
			{
				alPair<alListNode<_vertex_data>*, alListNode<_vertex_data>*> target;
				target.m_first = tgCurr;
				target.m_second = tgCurr->m_right;

				if (target.m_first == prev.m_first)
					break;

				/*miRay r;
				r.m_origin = current.m_first->m_data.m_vertex->m_position;
				r.m_end = current.m_second->m_data.m_vertex->m_position;

				float32_t d = r.distanceToLine(target.m_first->m_data.m_vertex->m_position, target.m_second->m_data.m_vertex->m_position);
				if (d < lineLineCollisionLen)*/
				alVec4f normal;
				alTriangle tri1;
				alTriangle tri2;
				{
					alTriangle tri3;
					auto vertex = current.m_first;

					tri3.v1 = vertex->m_data.m_vertex->m_position;
					vertex = vertex->m_right;

					tri3.v2 = vertex->m_data.m_vertex->m_position;
					vertex = vertex->m_right;

					tri3.v3 = vertex->m_data.m_vertex->m_position;

					tri3.Update();
					tri3.e1.Cross2(tri3.e2, normal);
					normal.Normalize2();
				}

				tri1.v1 = current.m_first->m_data.m_vertex->m_position;
				tri1.v2 = current.m_first->m_right->m_data.m_vertex->m_position;
				tri1.v3 = current.m_first->m_data.m_vertex->m_position;

				tri2.v1 = current.m_first->m_data.m_vertex->m_position;
				tri2.v2 = current.m_first->m_right->m_data.m_vertex->m_position;
				tri2.v3 = current.m_first->m_right->m_data.m_vertex->m_position;

				//ip = m_origin + t * m_direction;
				tri1.v1 = tri1.v1 + 1.f * normal;
				tri1.v2 = tri1.v2 + 1.f * normal;
				tri1.v3 = tri1.v3 + 1.f * -normal;

				tri2.v1 = tri2.v1 + 1.f * -normal;
				tri2.v2 = tri2.v2 + 1.f * -normal;
				tri2.v3 = tri2.v3 + 1.f * normal;

				//{ // scale
				//	float32_t sz = 100.f;
				//	v4f center;
				//	tri.center(center);

				//	tri.v1 = ((tri.v1 - center) * sz) + center;
				//	tri.v2 = ((tri.v2 - center) * sz) + center;
				//	tri.v3 = ((tri.v3 - center) * sz) + center;
				//}
				tri1.Update();
				tri2.Update();

				alRay r;
				r.m_origin = target.m_first->m_data.m_vertex->m_position;
				r.m_end = target.m_second->m_data.m_vertex->m_position;
				r.Update();

				float32_t T = 0.f;
				float32_t U = 0.f;
				float32_t V = 0.f;
				float32_t W = 0.f;

				bool good = tri1.RayTest_MT(r, true, T, U, V, W);

				if (!good)
					good = tri2.RayTest_MT(r, true, T, U, V, W);

				if (good)
				{
					auto tmp = target.m_first->m_data.m_vertex;
					target.m_first->m_data.m_vertex = current.m_second->m_data.m_vertex;
					current.m_second->m_data.m_vertex = tmp;
					goto end;
				}

				if (tgCurr == tgLast)
					break;

				tgCurr = tgCurr->m_right;
			}


			if (curV == lastV)
				break;
			curV = curV->m_right;
		}
	end:;
	}
}

// ==============================================================
// 
// 
//                                                  alPolygonMesh
// 
// 
// ==============================================================
alPolygonMesh<>::alPolygonMesh()
{
}

alPolygonMesh<>::~alPolygonMesh() 
{
	Clear();
}

template<>
void alPolygonMesh<>::Clear()
{
	if (m_first_polygon)
	{
		auto p = m_first_polygon;
		auto last = p->m_left;
		while (true) 
		{
			auto next = p->m_right;
			p->~alPolygon();
			m_allocator.Deallocate<alPolygon>(p);
			if (p == last)
				break;
			p = next;
		}

		m_first_polygon = 0;
	}


	if (m_first_vertex)
	{
		auto v = m_first_vertex;
		auto last = v->m_left;
		while (true) 
		{
			auto next = v->m_right;
			v->~alVertex();
			m_allocator.Deallocate<alVertex>(v);
			if (v == last)
				break;
			v = next;
		}
		m_first_vertex = 0;
	}
}

void alPolygonMesh<>::AddCube(float32_t size, const alMat4& W)
{

	float32_t pos = size;

	alPolygonCreator pc;
	pc.Clear();
	pc.SetPosition(alVec3f(-pos, pos, -pos));
	pc.SetUV(alVec2f(0.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, pos, pos));
	pc.SetUV(alVec2f(0.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, pos, pos));
	pc.SetUV(alVec2f(1.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, pos, -pos));
	pc.SetUV(alVec2f(1.f, 0.f));
	pc.Add(false);
	pc.ApplyMatrix(W);
	AddPolygon(&pc);

	pc.Clear();
	pc.SetPosition(alVec3f(-pos, -pos, pos));
	pc.SetUV(alVec2f(1.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, -pos, -pos));
	pc.SetUV(alVec2f(1.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, -pos, -pos));
	pc.SetUV(alVec2f(0.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, -pos, pos));
	pc.SetUV(alVec2f(0.f, 1.f));
	pc.Add(false);
	pc.ApplyMatrix(W);
	AddPolygon(&pc);

	pc.Clear();
	pc.SetPosition(alVec3f(pos, -pos, pos));
	pc.SetUV(alVec2f(1.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, pos, pos));
	pc.SetUV(alVec2f(1.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, pos, pos));
	pc.SetUV(alVec2f(0.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, -pos, pos));
	pc.SetUV(alVec2f(0.f, 1.f));
	pc.Add(false);
	pc.ApplyMatrix(W);
	AddPolygon(&pc);

	pc.Clear();
	pc.SetPosition(alVec3f(pos, pos, -pos));
	pc.SetUV(alVec2f(0.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, -pos, -pos));
	pc.SetUV(alVec2f(0.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, -pos, -pos));
	pc.SetUV(alVec2f(1.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, pos, -pos));
	pc.SetUV(alVec2f(1.f, 0.f));
	pc.Add(false);
	pc.ApplyMatrix(W);
	AddPolygon(&pc);

	pc.Clear();
	pc.SetPosition(alVec3f(pos, pos, pos));
	pc.SetUV(alVec2f(0.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, -pos, pos));
	pc.SetUV(alVec2f(0.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, -pos, -pos));
	pc.SetUV(alVec2f(1.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(pos, pos, -pos));
	pc.SetUV(alVec2f(1.f, 0.f));
	pc.Add(false);
	pc.ApplyMatrix(W);
	AddPolygon(&pc);

	pc.Clear();
	pc.SetPosition(alVec3f(-pos, -pos, pos));
	pc.SetUV(alVec2f(1.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, pos, pos));
	pc.SetUV(alVec2f(1.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, pos, -pos));
	pc.SetUV(alVec2f(0.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-pos, -pos, -pos));
	pc.SetUV(alVec2f(0.f, 1.f));
	pc.Add(false);
	pc.ApplyMatrix(W);
	AddPolygon(&pc);
}


void alPolygonMesh<>::AddSphere(uint32_t _segments, float32_t radius, const alMat4& W)
{
	alPolygonCreator pc;

	if (_segments < 3) _segments = 3;
	if (_segments > 32) _segments = 32;

	uint32_t segments = _segments;
	uint32_t segmentsXi = segments - 1;

	uint32_t lastSegment = segments-1;

	float32_t angleStepY = 360.f / float32_t(segments);
	float32_t angleStepX = 180.f / float32_t(segments-1);

	float32_t angleY = 0.f;
	float32_t angleX = 0.f;
	//printf("angleStepY [%f]\n", angleStepY);
	//printf("angleStepX [%f]\n", angleStepX);

	alVec3f p3prev;
	alVec3f p4prev;
	
	alVec3f p_base(0.f, radius, 0.f);

	float32_t uvstep = 1.f / float32_t(segments);
	float32_t currU = 1.f;
	float32_t currV = 0;
	bool weld = true;

	for (uint32_t yi = 0; yi < segments; ++yi)
	{
		for (uint32_t xi = 0; xi < segmentsXi; ++xi)
		{
			alMat4 mX(alQuaternion(alMath::DegToRad(angleX), 0.f, 0.f));
			alVec3f p1_base = alMath::Mul(p_base, mX);
			alMat4 mXn(alQuaternion(alMath::DegToRad(angleX + angleStepX), 0.f, 0.f));
			alVec3f p2_base = alMath::Mul(p_base, mXn);

			if (xi == 0)
			{
				// in this branch I can create pyramids and cones				
				alMat4 mY(alQuaternion(0.f, alMath::DegToRad(angleY), 0.f));
				alMat4 mYn(alQuaternion(0.f, alMath::DegToRad(angleY + angleStepY), 0.f));

				alVec3f p1 = p1_base;
				alVec3f p2 = alMath::Mul(p2_base, mY);
				alVec3f p3 = alMath::Mul(p2_base, mYn);

				pc.Clear();
				pc.SetPosition(p1);
				pc.SetUV(0.5f, 0.f);
				pc.Add(weld);
				pc.SetPosition(p3);
				pc.SetUV(currU - uvstep, currV + uvstep);
				pc.Add(weld);
				pc.SetPosition(p2);
				pc.SetUV(currU, currV + uvstep);
				pc.Add(weld);
				pc.ApplyMatrix(W);
				AddPolygon(&pc);
			}
			else if (xi == segmentsXi-1)
			{
				alMat4 mY(alQuaternion(0.f, alMath::DegToRad(angleY), 0.f));
				alMat4 mYn(alQuaternion(0.f, alMath::DegToRad(angleY + angleStepY), 0.f));

				alVec3f p1 = alMath::Mul(p1_base, mY);
				alVec3f p2 = p2_base;
				alVec3f p3 = alMath::Mul(p1_base, mYn);


				// need to fix UV in this part

				pc.Clear();
				pc.SetPosition(p1);
				pc.SetUV(currU, currV);
				pc.Add(weld);
				pc.SetPosition(p3);
				pc.SetUV(currU - uvstep, currV);
				pc.Add(weld);
				pc.SetPosition(p2);
				pc.SetUV(0.5f, 1.f);
				pc.Add(weld);
				pc.ApplyMatrix(W);
				AddPolygon(&pc);
			}
			else
			{
				alMat4 mY(alQuaternion(0.f, alMath::DegToRad(angleY), 0.f));
				alMat4 mYn(alQuaternion(0.f, alMath::DegToRad(angleY + angleStepY), 0.f));

				alVec3f p1 = alMath::Mul(p1_base, mY);
				alVec3f p2 = alMath::Mul(p2_base, mY);
				alVec3f p3 = alMath::Mul(p2_base, mYn);
				alVec3f p4 = alMath::Mul(p1_base, mYn);
				/*if (yi > 0)
					p2 = p3prev;
				else
					p2 = alMath::Mul(p1, mX);
				p3prev = p3;*/

				pc.Clear();
				pc.SetPosition(p2);
				pc.SetUV(currU, currV + uvstep);
				pc.Add(weld);
				pc.SetPosition(p1);
				pc.SetUV(currU, currV);
				pc.Add(weld);
				pc.SetPosition(p4);
				pc.SetUV(currU - uvstep, currV);
				pc.Add(weld);
				pc.SetPosition(p3);
				pc.SetUV(currU - uvstep, currV + uvstep);
				pc.Add(weld);
				pc.ApplyMatrix(W);
				AddPolygon(&pc);
			}
		
			angleX += angleStepX;
			currV += uvstep;
		}
		
		currV = 0.f;
		currU -= uvstep;

		angleX = 0.f;
		angleY += angleStepY;
	}

	// delete this later
	//alArray<alPolygon*> invisiblePolygons;
	//auto c = m_first_polygon;
	//if (c)
	//{
	//	auto l = c->m_left;
	//	while (true)
	//	{
	//		if (!c->IsVisible())
	//		{
	//			printf("A\n");
	//			invisiblePolygons.push_back(c);
	//		}
	//		if (c == l)
	//			break;
	//		c = c->m_right;
	//	}
	//}
	//if (invisiblePolygons.size())
	//{
	//	for (size_t i = 0; i < invisiblePolygons.m_size; ++i)
	//	{
	//		//DeletePolygon(invisiblePolygons.m_data[i]);
	//	}
	//}
}

void alPolygonMesh<>::UpdateCounts()
{
	m_vertexCount = 0;
	m_polygonCount = 0;
	{
		auto c = m_first_vertex;
		if (c)
		{
			auto l = c->m_left;
			while (true)
			{
				++m_vertexCount;
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}
	{
		auto c = m_first_polygon;
		if (c)
		{
			auto l = c->m_left;
			while (true)
			{
				++m_polygonCount;
				auto cv = c->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					//++m_uvCount;
					if (cv == lv)
						break;
					cv = cv->m_right;
				}
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}
}

void alPolygonMesh<>::_add_vertex_to_list(alVertex* newVertex)
{
	if (!m_first_vertex)
	{
		m_first_vertex = newVertex;
		m_first_vertex->m_right = m_first_vertex;
		m_first_vertex->m_left = m_first_vertex;
	}
	else
	{
		auto last = m_first_vertex->m_left;
		last->m_right = newVertex;
		newVertex->m_left = last;
		newVertex->m_right = m_first_vertex;
		m_first_vertex->m_left = newVertex;
	}
}

void alPolygonMesh<>::_add_polygon_to_list(alPolygon* newPolygon)
{
	if (!m_first_polygon)
	{
		m_first_polygon = newPolygon;
		m_first_polygon->m_right = m_first_polygon;
		m_first_polygon->m_left = m_first_polygon;
	}
	else
	{
		auto last = m_first_polygon->m_left;
		last->m_right = newPolygon;
		newPolygon->m_left = last;
		newPolygon->m_right = m_first_polygon;
		m_first_polygon->m_left = newPolygon;
	}
}

void alPolygonMesh<>::_remove_vertex_from_list(alVertex* o)
{
	auto l = o->m_left;
	auto r = o->m_right;
	l->m_right = r;
	r->m_left = l;

	if (o == m_first_vertex)
		m_first_vertex = o->m_right;
	if (o == m_first_vertex)
		m_first_vertex = 0;
}

void alPolygonMesh<>::_remove_polygon_from_list(alPolygon* o)
{
	auto l = o->m_left;
	auto r = o->m_right;
	l->m_right = r;
	r->m_left = l;

	if (o == m_first_polygon)
		m_first_polygon = o->m_right;
	if (o == m_first_polygon)
		m_first_polygon = 0;
}

template<>
void alPolygonMesh<>::AddPolygon(alPolygonCreator* pc)
{
	AL_ASSERT_ST(pc);

	if (!pc->IsVisible())
	{
	//	return;
	}

	int polygonVertexCount = pc->Size();
	if (polygonVertexCount > 2)
	{
		alPolygon* newPolygon = alCreate<alPolygon>();
		if (!m_first_polygon)
		{
			m_first_polygon = newPolygon;
			m_first_polygon->m_right = m_first_polygon;
			m_first_polygon->m_left = m_first_polygon;
		}
		else
		{
			auto last = m_first_polygon->m_left;
			last->m_right = newPolygon;
			newPolygon->m_left = last;
			newPolygon->m_right = m_first_polygon;
			m_first_polygon->m_left = newPolygon;
		}

		auto positions = pc->GetPositions();
		auto normals = pc->GetNormals();
		auto tCoords = pc->GetTCoords();
		auto binormals = pc->GetBinormals();
		auto tangents = pc->GetTangents();
		
	/*	alPolygon testPolygon;
		for (int i = 0; i < polygonVertexCount; ++i)
		{
			alPolygon::_vertex_data newVD;
			newVD.m_vertex = newVertex;
			testPolygon.m_verts.push_back();
		}*/

		for (int i = 0; i < polygonVertexCount; ++i)
		{
			alVertex* newVertex = 0;
			m_aabb.Add(positions[i].m_first);

			if (positions[i].m_second & alPolygonCreator::flag_weld)
			{
				_set_hash(&positions[i].m_first);

				auto find_result = m_weldMap.find(m_vertsMapHash);
				if (find_result == m_weldMap.end())
				{
					//newVertex = m_allocatorVertex->Allocate();
					//new(newVertex)miVertex();
					newVertex = alCreate<alVertex>();

					newVertex->m_position = positions[i].m_first;

					m_weldMap[m_vertsMapHash] = newVertex;
					_add_vertex_to_list(newVertex);
				}
				else
				{
					newVertex = find_result->second;
				}
			}
			else
			{
				//newVertex = m_allocatorVertex->Allocate();
				//new(newVertex)miVertex();
				newVertex = alCreate<alVertex>();

				newVertex->m_position = positions[i].m_first;

				_add_vertex_to_list(newVertex);
			}

			// add newPolygon to vertex and newVertex to polygon
			// only once
			if (newVertex->m_polygons.find(newPolygon) == 0)
			{
				newVertex->m_polygons.push_back(newPolygon);
				alPolygon::_vertex_data newVD;
				newVD.m_vertex = newVertex;
				newVD.m_uv = tCoords[i];
				newVD.m_normal = normals[i];
				newVD.m_binormal = binormals[i];
				newVD.m_tangent = tangents[i];

				newPolygon->m_verts.push_back(newVD);
			}
		}

//		newPolygon->CalculateNormal();
	}
}

template<>
void alPolygonMesh<>::_set_hash(alVec3f* position)
{
	m_vertsMapHash.clear();

	char buff[2000];
	sprintf_s(buff, 2000, "%.6f%.6f%.6f", position->x, position->y, position->z);
	m_vertsMapHash = buff;
}

alMesh* alPolygonMesh<>::CreateMesh()
{
	alMesh* newMesh = 0;
	uint32_t numOfVerts = 0;

	if (m_first_polygon)
	{

		auto c = m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			//++m_polygonCount; just to know
			
			uint32_t numVertPerPolygon = 0;
			auto cv = c->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				//++m_uvCount;
				++numVertPerPolygon;

				if (cv == lv)
					break;
				cv = cv->m_right;
			}

			uint32_t numOfTris = numVertPerPolygon - 2;

			numOfVerts += numOfTris * 3;

			if (c == l)
				break;
			c = c->m_right;
		}

		if (numOfVerts > 2)
		{
			newMesh = alCreate<alMesh>();
			newMesh->m_aabb = m_aabb;
			newMesh->m_iCount = newMesh->m_vCount = numOfVerts;
			
			uint32_t indexCounter = 0;

			uint32_t* inds32 = 0;
			uint16_t* inds16 = 0;
			if (newMesh->m_iCount > 0xffff)
			{
				newMesh->m_indexType = alMeshIndexType::u32;
				newMesh->m_indices = (uint8_t*)alMemory::Malloc(newMesh->m_iCount * sizeof(uint32_t));
				inds32 = (uint32_t*)newMesh->m_indices;
			}
			else
			{
				newMesh->m_indices = (uint8_t*)alMemory::Malloc(newMesh->m_iCount * sizeof(uint16_t));
				inds16 = (uint16_t*)newMesh->m_indices;
			}

			newMesh->m_vertexType = alMeshVertexType::Triangle;
			newMesh->m_stride = sizeof(alVertexTriangle);
			newMesh->m_vertices = (uint8_t*)alMemory::Malloc(newMesh->m_vCount * newMesh->m_stride);
			for (uint32_t i = 0; i < newMesh->m_iCount; ++i)
			{
				if (newMesh->m_indexType == alMeshIndexType::u32)
				{
					*inds32 = indexCounter;
					++inds32;
					++indexCounter;
				}
				else
				{
					*inds16 = indexCounter;
					++inds16;
					++indexCounter;
				}
			}
			
			alVertexTriangle* meshVerts = (alVertexTriangle*)newMesh->m_vertices;

			/*c = m_first_polygon;
			l = c->m_left;
			while (true)
			{
				auto cv = c->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					meshVerts->Position = cv->m_data.m_vertex->m_position;
					meshVerts->UV1 = cv->m_data.m_uv;
					++meshVerts;

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
				if (c == l)
					break;
				c = c->m_right;
			}*/
			auto current_polygon = m_first_polygon;
			auto last_polygon = current_polygon->m_left;
			while (true)
			{
				auto vertex_1 = current_polygon->m_verts.m_head;
				auto vertex_3 = vertex_1->m_right;
				auto vertex_2 = vertex_3->m_right;
				while (true)
				{
					meshVerts->Position = vertex_1->m_data.m_vertex->m_position;
					meshVerts->UV1 = vertex_1->m_data.m_uv;
					meshVerts->Normal = vertex_1->m_data.m_normal;
					meshVerts->Binormal = vertex_1->m_data.m_binormal;
					meshVerts->Tangent = vertex_1->m_data.m_tangent;
					++meshVerts;

					meshVerts->Position = vertex_2->m_data.m_vertex->m_position;
					meshVerts->UV1 = vertex_2->m_data.m_uv;
					meshVerts->Normal = vertex_2->m_data.m_normal;
					meshVerts->Binormal = vertex_2->m_data.m_binormal;
					meshVerts->Tangent = vertex_2->m_data.m_tangent;
					++meshVerts;

					meshVerts->Position = vertex_3->m_data.m_vertex->m_position;
					meshVerts->UV1 = vertex_3->m_data.m_uv;
					meshVerts->Normal = vertex_3->m_data.m_normal;
					meshVerts->Binormal = vertex_3->m_data.m_binormal;
					meshVerts->Tangent = vertex_3->m_data.m_tangent;
					++meshVerts;

					vertex_2 = vertex_2->m_right;
					vertex_3 = vertex_3->m_right;

					if (vertex_2 == vertex_1)
						break;
				}
				if (current_polygon == last_polygon)
					break;
				current_polygon = current_polygon->m_right;
			}
		}
	}

	//printf("numOfVerts: %u\n", numOfVerts);

	return newMesh;
}

void alPolygonMesh<>::GenerateNormals(bool smooth)
{
	auto current_polygon = m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true)
	{
		current_polygon->CalculateNormal();
		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}

	if (smooth)
	{
		auto curr = m_first_vertex;
		auto last = curr->m_left;
		while (true)
		{
			uint32_t nn = 0;
			alVec3f normal;
			{
				auto cp = curr->m_polygons.m_head;
				auto lp = cp->m_left;
				while (true)
				{

					auto vNode = cp->m_data->FindVertex(curr);
					if (vNode)
					{
						normal += cp->m_data->GetFaceNormal();
						//normal += vNode->m_data.m_normal;
						++nn;
					}

					if (cp == lp)
						break;
					cp = cp->m_right;
				}
			}

		//	printf(" %u", nn);
			if(nn)
				normal = normal / (float32_t)nn;
			normal.Normalize2();

		////	auto node = curr->m_polygons.m_head->m_data->FindVertex(curr);
		////	node->m_data.m_normal = normal;

			//normal.Normalize2();
			{
				auto cp = curr->m_polygons.m_head;
				auto lp = cp->m_left;
				while (true)
				{
					auto vNode = cp->m_data->FindVertex(curr);
					if (vNode)
						vNode->m_data.m_normal = normal;
					if (cp == lp)
						break;
					cp = cp->m_right;
				}
			}

			if (curr == last)
				break;
			curr = curr->m_right;
		}
	}
}

void alPolygonMesh<>::DeletePolygon(alPolygon* p)
{
}