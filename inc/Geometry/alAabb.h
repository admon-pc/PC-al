#ifndef _AL_AABBH_
#define _AL_AABBH_

// Axis-Aligned Bounding Box
class alAabb
{
public:

	alAabb();
	alAabb(const alVec4& min, const alVec4& max);
	alAabb(const alVec4f& min, const alVec4f& max);

	void Transform(alAabb* original, alMat4* matrix, alVec4* position);
	void Add(const alVec4& point);
	void Add(const alVec4f& point);
	void Add(const alVec3f& point);
	void Add(const alAabb& box);
	bool RayTest(const alRay& r);
	void Center(alVec4& v) const;
	float64_t Radius();
	void Extent(alVec4& v);
	bool IsEmpty() const;
	void Reset();

	alVec4 m_min;
	alVec4 m_max;
};

#endif

