#ifndef _AL_POLYGONMESHH_
#define _AL_POLYGONMESHH_

#include "Containers/alList.h"
#include "Geometry/alMesh.h"

#include <string>
#include <map>

class alPolygon;
class alVertex
{
public:

	alVertex() {}
	~alVertex() {}

	alVertex* m_left = 0;
	alVertex* m_right = 0;

	alVec3f m_position;

	void CopyData(alVertex* other)
	{
		m_position = other->m_position;
	}

	alList<alPolygon*> m_polygons;
};

class alPolygon
{
public:

	alPolygon();
	~alPolygon();

	void CopyData(alPolygon* other);

	alPolygon* m_left = 0;
	alPolygon* m_right = 0;

	//miList3<alVertex*, alVec2f, alVec3f> m_verts;
	struct _vertex_data 
	{
		_vertex_data() :m_vertex(0), m_flags(0) {}
		_vertex_data(alVertex* v) :m_vertex(v), m_flags(0) {}
		_vertex_data(alVertex* v, const alVec2f& uv, const alVec3f& normal, uint32_t flags)
			:
			m_vertex(v),
			m_uv(uv),
			m_normal(normal),
			m_flags(flags)
		{}

		bool operator==(const _vertex_data& other) {
			return m_vertex == other.m_vertex;
		}

		alVertex* m_vertex;
		alVec2f m_uv;
		alVec3f m_normal;
		alVec3f m_binormal;
		alVec3f m_tangent;
		uint32_t m_flags;
	};
	alList<_vertex_data> m_verts;

	alVec3f m_faceNormal;

	alListNode<_vertex_data>* FindVertex(alVertex* v);
	bool IsVisible();
	void CalculateNormal();
	alVec3f GetFaceNormal();
	alVec3f GetFaceNormalCalculateNew();
	void Flip();
	void FixOrder(float32_t lineLineCollisionLen);
};

// create alPolygonCreator by yourself
// use it when you create new model
// don't forget to call Clear() before\after every polygon
class alPolygonCreator
{
	uint32_t m_size = 0;
	uint32_t m_allocated = 0;
	alPair<alVec3f, uint8_t>* m_positions = 0;
	alVec3f* m_normals = 0;
	alVec3f* m_binormals = 0;
	alVec3f* m_tangents = 0;
	alVec2f* m_tcoords = 0;

	void _reallocate(uint32_t size) {
		alPair<alVec3f, uint8_t>* newPositions = new alPair<alVec3f, uint8_t>[size];
		alVec3f* newNormals = new alVec3f[size];
		alVec3f* newBinormals = new alVec3f[size];
		alVec3f* newTangents = new alVec3f[size];
		alVec2f* newtCoords = new alVec2f[size];

		for (uint32_t i = 0; i < m_size; ++i)
		{
			newPositions[i] = m_positions[i];
			newNormals[i] = m_normals[i];
			newtCoords[i] = m_tcoords[i];
			newBinormals[i] = m_binormals[i];
			newTangents[i] = m_tangents[i];
		}

		delete[] m_positions;
		delete[] m_normals;
		delete[] m_tcoords;
		delete[] m_binormals;
		delete[] m_tangents;

		m_positions = newPositions;
		m_normals = newNormals;
		m_tcoords = newtCoords;
		m_binormals = newBinormals;
		m_tangents = newTangents;

		m_allocated = size;
	}

	alVec3f m_currPosition;
	alVec3f m_currNormal;
	alVec3f m_currBinormal;
	alVec3f m_currTangent;
	alVec2f m_currUV;
public:
	alPolygonCreator() {}
	~alPolygonCreator() 
	{
		if (m_positions) delete[] m_positions;
		if (m_normals) delete[] m_normals;
		if (m_tcoords) delete[] m_tcoords;
		if (m_binormals) delete[] m_binormals;
		if (m_tangents) delete[] m_tangents;
	}

	void Clear() { m_size = 0; }
	int Size() { return m_size; }

	enum
	{
		flag_weld = BIT(0),
	};

	void SetPosition(const alVec3f& v) { m_currPosition = v; }
	void SetNormal(const alVec3f& v) { m_currNormal = v; }
	void SetBinormal(const alVec3f& v) { m_currBinormal = v; }
	void SetTangent(const alVec3f& v) { m_currTangent = v; }
	void SetUV(const alVec2f& v) { m_currUV = v; }
	void SetUV(float32_t u, float32_t v) { m_currUV.x = u; m_currUV.y = v; }
	void Add(bool weld)
	{
		uint8_t flags = 0;
		if (weld)
			flags |= flag_weld;

		if (m_size == m_allocated)
		{
			_reallocate(m_allocated + (int)std::ceil(((2.f + (float)m_allocated) * 0.5f)));
		}
		m_positions[m_size] = alPair<alVec3f, uint8_t>(m_currPosition, flags);
		m_normals[m_size] = m_currNormal;
		m_tcoords[m_size] = m_currUV;
		m_binormals[m_size] = m_currBinormal;
		m_tangents[m_size] = m_currTangent;
		++m_size;
	}
	void ApplyMatrix(const alMat4& W)
	{
		auto positions = GetPositions();
		for (uint32_t i = 0; i < m_size; ++i)
		{
			positions[i].m_first = alMath::Mul(positions[i].m_first, W);
		}
	}

	void Add(const alVec3f& position, bool weld, bool selected, const alVec3f& normal, const alVec2f& tCoords) {
		if (m_size == m_allocated)
		{
			_reallocate(m_allocated + (int)std::ceil(((2.f + (float)m_allocated) * 0.5f)));
		}
		uint8_t flags = 0;
		if (weld)
			flags |= flag_weld;

		m_positions[m_size] = alPair<alVec3f, uint8_t>(position, flags);
		m_normals[m_size] = normal;
		m_tcoords[m_size] = tCoords;
		++m_size;
	}

	bool IsVisible()
	{
		if (m_size > 2)
		{
			uint32_t vi = 1;
			auto vertex_1 = m_positions[0];
			

			uint32_t triNum = m_size - 2;

			for(uint32_t i = 0; i < triNum; ++i)
			{
				auto vertex_3 = m_positions[vi];
				auto vertex_2 = m_positions[vi + 1];

				auto a = vertex_2.m_first - vertex_1.m_first;
				auto b = vertex_3.m_first - vertex_1.m_first;

				alVec3f n;
				a.Cross2(b, n);

				float32_t area = 0.5f * sqrt(n.Dot());

				if (area > 0.001)
					return true;

				vi += 1;
			}
		}
		return false;
	}

	alPair<alVec3f, uint8_t>* GetPositions() { return m_positions; }
	alVec3f* GetNormals() { return m_normals; }
	alVec3f* GetBinormals() { return m_binormals; }
	alVec3f* GetTangents() { return m_tangents; }
	alVec2f* GetTCoords() { return m_tcoords; }
};

template<typename _allocator_type = alAllocatorDefault>
class alPolygonMesh
{
	void _add_vertex_to_list(alVertex* newVertex);
	void _add_polygon_to_list(alPolygon* newPolygon);
	void _remove_vertex_from_list(alVertex* o);
	void _remove_polygon_from_list(alPolygon* o);
	void _set_hash(alVec3f* position);

	_allocator_type m_allocator;

	std::map<std::string, alVertex*> m_weldMap;
	std::string m_vertsMapHash;
public:
	alPolygonMesh();
	~alPolygonMesh();

	void Clear();
	void AddCube(float32_t size, const alMat4&);
	void AddSphere(uint32_t segments, float32_t radius, const alMat4&);

	alPolygon* m_first_polygon = 0;
	alVertex* m_first_vertex = 0;
	//miSkeleton* m_skeleton = 0;

	uint32_t m_vertexCount = 0;
	uint32_t m_polygonCount = 0;

	alAabb m_aabb;

	void UpdateCounts();

	void AddPolygon(alPolygonCreator*);
	alMesh* CreateMesh();

	void GenerateNormals(bool smooth);
	void DeletePolygon(alPolygon*);
};


#endif

