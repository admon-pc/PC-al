#ifndef _AL_RAYH_
#define _AL_RAYH_

class alRay
{
public:
	alRay();
	~alRay();

	alVec4 m_origin;
	alVec4 m_end;
	alVec4 m_direction;
	alVec4 m_invDir;

	int32_t m_kz = 0;
	int32_t m_kx = 0;
	int32_t m_ky = 0;

	float32_t m_Sx = 0.f;
	float32_t m_Sy = 0.f;
	float32_t m_Sz = 0.f;

	void CreateFrom2DCoords(const alVec2i& coord, const alVec4i& rc, const alVec2i& rc_sz, const alMat4& VPinv);
	float32_t DistanceToLine(const alVec4& lineP0, const alVec4& lineP1);
	void Update();
	void GetIntersectionPoint(int32_t t, alVec4& ip);
	bool PlaneIntersection(const alVec4& planePoint, const alVec4f& planeNormal, float64_t& T);
};

#endif

