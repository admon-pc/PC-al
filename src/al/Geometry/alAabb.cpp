#include "al.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;


alAabb::alAabb()
	:
	m_min(alVec4FltMax),
	m_max(alVec4FltMaxNeg)
{}

alAabb::alAabb(const alVec4& min, const alVec4& max) : m_min(min), m_max(max) { }
alAabb::alAabb(const alVec4f& min, const alVec4f& max) 
{ 
	m_min.x = min.x;
	m_min.y = min.y;
	m_min.z = min.z;
}

	/*
	Transforming Axis-Aligned Bounding Boxes
	by Jim Arvo
	from "Graphics Gems", Academic Press, 1990
	*/
void alAabb::Transform(alAabb* original, alMat4* matrix, alVec4* position)
{
	float64_t  a, b;
	float64_t  Amin[3], Amax[3];
	float64_t  Bmin[3], Bmax[3];
	int32_t    i, j;

	/*Copy box A into a min array and a max array for easy reference.*/
	Amin[0] = original->m_min.x;  Amax[0] = original->m_max.x;
	Amin[1] = original->m_min.y;  Amax[1] = original->m_max.y;
	Amin[2] = original->m_min.z;  Amax[2] = original->m_max.z;

	/* Take care of translation by beginning at T. */
	Bmin[0] = Bmax[0] = position->x;
	Bmin[1] = Bmax[1] = position->y;
	Bmin[2] = Bmax[2] = position->z;

	/* Now find the extreme points by considering the product of the */
	/* min and max with each component of M.  */
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// mimi: I swapped j and i
			a = (float32_t)(matrix->m_data[j][i] * Amin[j]);
			b = (float32_t)(matrix->m_data[j][i] * Amax[j]);
			if (a < b)
			{
				Bmin[i] += a;
				Bmax[i] += b;
			}
			else
			{
				Bmin[i] += b;
				Bmax[i] += a;
			}
		}
	}
	/* Copy the result into the new box. */
	m_min.x = Bmin[0];  m_max.x = Bmax[0];
	m_min.y = Bmin[1];  m_max.y = Bmax[1];
	m_min.z = Bmin[2];  m_max.z = Bmax[2];
}

void alAabb::Add(const alVec4& point)
{
	if (point.x < m_min.x) m_min.x = point.x;
	if (point.y < m_min.y) m_min.y = point.y;
	if (point.z < m_min.z) m_min.z = point.z;

	if (point.x > m_max.x) m_max.x = point.x;
	if (point.y > m_max.y) m_max.y = point.y;
	if (point.z > m_max.z) m_max.z = point.z;
}
void alAabb::Add(const alVec4f& point)
{
	if (point.x < m_min.x) m_min.x = point.x;
	if (point.y < m_min.y) m_min.y = point.y;
	if (point.z < m_min.z) m_min.z = point.z;

	if (point.x > m_max.x) m_max.x = point.x;
	if (point.y > m_max.y) m_max.y = point.y;
	if (point.z > m_max.z) m_max.z = point.z;
}
void alAabb::Add(const alVec3f& point)
{
	if (point.x < m_min.x) m_min.x = point.x;
	if (point.y < m_min.y) m_min.y = point.y;
	if (point.z < m_min.z) m_min.z = point.z;

	if (point.x > m_max.x) m_max.x = point.x;
	if (point.y > m_max.y) m_max.y = point.y;
	if (point.z > m_max.z) m_max.z = point.z;
}

void alAabb::Add(const alAabb& box)
{
	if (box.m_min.x < m_min.x) m_min.x = box.m_min.x;
	if (box.m_min.y < m_min.y) m_min.y = box.m_min.y;
	if (box.m_min.z < m_min.z) m_min.z = box.m_min.z;

	if (box.m_max.x > m_max.x) m_max.x = box.m_max.x;
	if (box.m_max.y > m_max.y) m_max.y = box.m_max.y;
	if (box.m_max.z > m_max.z) m_max.z = box.m_max.z;
}

bool alAabb::RayTest(const alRay& r)
{
	float64_t t1 = (m_min.x - r.m_origin.x) * r.m_invDir.x;
	float64_t t2 = (m_max.x - r.m_origin.x) * r.m_invDir.x;
	float64_t t3 = (m_min.y - r.m_origin.y) * r.m_invDir.y;
	float64_t t4 = (m_max.y - r.m_origin.y) * r.m_invDir.y;
	float64_t t5 = (m_min.z - r.m_origin.z) * r.m_invDir.z;
	float64_t t6 = (m_max.z - r.m_origin.z) * r.m_invDir.z;

	float64_t tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float64_t tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0 || tmin > tmax) return false;

	return true;
}

void alAabb::Center(alVec4& v) const
{
	v = alVec4(m_min + m_max);
	v *= 0.5f;
}

float64_t alAabb::Radius() 
{
	return m_min.Distance(m_max) * 0.5f;
}

void alAabb::Extent(alVec4& v)
{
	v = alVec4(m_max - m_min);
}

bool alAabb::IsEmpty() const 
{
	return ((m_min == alVec4FltMax) && (m_max == alVec4FltMaxNeg))
		|| (m_min == m_max);
}

void alAabb::Reset()
{
	m_min = alVec4FltMax;
	m_max = alVec4FltMaxNeg;
}

