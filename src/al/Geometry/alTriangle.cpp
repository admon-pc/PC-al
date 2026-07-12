#include "al.h"
#include "Geometry/alTriangle.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alTriangle::alTriangle() 
{
}

alTriangle::alTriangle(const alVec4f& _v1, const alVec4f& _v2, const alVec4f& _v3)
	:
	v1(_v1),
	v2(_v2),
	v3(_v3)
{
	Update();
}

alTriangle::alTriangle(const alVec3f& _v1, const alVec3f& _v2, const alVec3f& _v3)
	:
	v1(_v1),
	v2(_v2),
	v3(_v3)
{
	Update();
}

void alTriangle::Update()
{
	e1 = alVec4f(v2.x - v1.x,
		v2.y - v1.y,
		v2.z - v1.z,
		0.f);
	e2 = alVec4f(v3.x - v1.x,
		v3.y - v1.y,
		v3.z - v1.z,
		0.f);
	//	e1.cross(e2, faceNormal);
}

void alTriangle::Center(alVec4f& out)
{
	out = (v1 + v2 + v3) * 0.3333333f;
}

bool alTriangle::RayTest_MT(const alRay& ray, bool withBackFace, float& T, float& U, float& V, float& W)
{
	alVec4  pvec;
	ray.m_direction.Cross2(alVec4(e2.x, e2.y, e2.z, e2.w), pvec);
	float det = e1.Dot(alVec4f((float32_t)pvec.x, (float32_t)pvec.y, (float32_t)pvec.z, (float32_t)pvec.w));

	if (withBackFace)
	{
		if (std::fabs(det) < alEpsilon)
			return false;
	}
	else
	{
		if (det < alEpsilon && det > -alEpsilon)
			return false;
	}

	alVec4 tvec(
		ray.m_origin.x - v1.x,
		ray.m_origin.y - v1.y,
		ray.m_origin.z - v1.z,
		0.f);

	float inv_det = 1.f / det;
	U = (float32_t)tvec.Dot(pvec) * inv_det;

	if (U < 0.f || U > 1.f)
		return false;

	alVec4  qvec;
	tvec.Cross2(alVec4(e1.x, e1.y, e1.z, e1.w), qvec);
	V = (float32_t)ray.m_direction.Dot(qvec) * inv_det;

	if (V < 0.f || U + V > 1.f)
		return false;

	T = e2.Dot(alVec4f((float32_t)qvec.x, (float32_t)qvec.y, (float32_t)qvec.z, (float32_t)qvec.w)) * inv_det;

	if (T < alEpsilon) return false;

	W = 1.f - U - V;
	return true;
}

bool alTriangle::RayTest_Watertight(const alRay& ray, bool withBackFace, float& T, float& U, float& V, float& W)
{
	v1.w = 1.f;
	v2.w = 1.f;
	v3.w = 1.f;
	const auto A = v2 - ray.m_origin;
	const auto B = v3 - ray.m_origin;
	const auto C = v1 - ray.m_origin;

	const float Ax = A[ray.m_kx] - (ray.m_Sx * A[ray.m_kz]);
	const float Ay = A[ray.m_ky] - (ray.m_Sy * A[ray.m_kz]);
	const float Bx = B[ray.m_kx] - (ray.m_Sx * B[ray.m_kz]);
	const float By = B[ray.m_ky] - (ray.m_Sy * B[ray.m_kz]);
	const float Cx = C[ray.m_kx] - (ray.m_Sx * C[ray.m_kz]);
	const float Cy = C[ray.m_ky] - (ray.m_Sy * C[ray.m_kz]);

	U = (Cx * By) - (Cy * Bx);
	V = (Ax * Cy) - (Ay * Cx);
	W = (Bx * Ay) - (By * Ax);

	if (U == 0.f || V == 0.f || W == 0.f)
	{
		double CxBy = (double)Cx * (double)By;
		double CyBx = (double)Cy * (double)Bx;
		U = (float)(CxBy - CyBx);

		double AxCy = (double)Ax * (double)Cy;
		double AyCx = (double)Ay * (double)Cx;
		V = (float)(AxCy - AyCx);

		double BxAy = (double)Bx * (double)Ay;
		double ByAx = (double)By * (double)Ax;
		W = (float)(BxAy - ByAx);
	}

	if (withBackFace)
	{
		if ((U < 0.f || V < 0.f || W < 0.f) &&
			(U > 0.f || V > 0.f || W > 0.f))
			return false;
	}
	else
	{
		if (U < 0.f || V < 0.f || W < 0.f)
			return false;
	}

	float det = U + V + W;

	if (det == 0.f)
		return false;

	const float Az = ray.m_Sz * A[ray.m_kz];
	const float Bz = ray.m_Sz * B[ray.m_kz];
	const float Cz = ray.m_Sz * C[ray.m_kz];
	const float Ts = (U * Az) + (V * Bz) + (W * Cz);

	if (!withBackFace) // CULL
	{
		if (Ts < 0.f || Ts > alInfinity * det)
			return false;
	}
	else
	{
		if (det < 0.f && (Ts >= 0.f || Ts < alInfinity * det))
			return false;
		else if (det > 0.f && (Ts <= 0.f || Ts > alInfinity * det))
			return false;
	}

	const float invDet = 1.f / det;
	U = U * invDet;
	V = V * invDet;
	W = W * invDet;
	T = Ts * invDet;
	if (T < alEpsilon)
		return false;
	return true;
}
