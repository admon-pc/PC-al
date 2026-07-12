#include "al.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

float32_t alMath::Distance2d(float32_t p1x,float32_t p1y,float32_t p2x,float32_t p2y)
{
	p1x -= p2x;
	p1y -= p2y;
	return sqrtf((p1x * p1x) + (p1y * p1y));
}
float32_t alMath::Distance2d(const alVec2f p1, const alVec2f p2)
{
	float32_t px = p1.x - p2.x;
	float32_t py = p1.y - p2.y;
	return sqrtf((px * px) + (py * py));
}

bool alMath::PointInRect(int32_t x, int32_t y, const alVec4i& rect)
{
	if (x >= rect.x) 
	{
		if (x <= rect.z)
		{
			if (y >= rect.y)
			{
				if (y <= rect.w) 
				{
					return true; 
				}
			}
		}
	}
	return false;
}

bool alMath::PointInRect(int32_t x, int32_t y, const alVec4f& rect)
{
	if (x >= rect.x)
	{
		if (x <= rect.z)
		{
			if (y >= rect.y)
			{
				if (y <= rect.w)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool alMath::PointInRect(float32_t x, float32_t y, const alVec4f& rect)
{
	if (x >= rect.x)
	{
		if (x <= rect.z)
		{
			if (y >= rect.y)
			{
				if (y <= rect.w)
				{
					return true;
				}
			}
		}
	}
	return false;
}

float32_t alMath::DegToRad(float32_t degrees)
{
	return degrees * (PIf / 180.f);
}

float32_t alMath::RadToDeg(float32_t radians)
{
	return radians * (180.f / PIf);
}

float32_t alMath::Clamp(float32_t x, float32_t min, float32_t max)
{
	if (x < min)
		return min;
	else if (x > max)
		return max;
	return x;
}

float32_t alMath::Lerp1(float32_t a, float32_t b, float32_t t)
{
	return (a * (1.f - t)) + (b * t);
}

float64_t alMath::Lerp1(float64_t a, float64_t b, float64_t t)
{
	return (a * (1.f - t)) + (b * t);
}

void alMath::Lerp1(const alColor& c1, const alColor& c2, float32_t t, alColor& out)
{
	out.m_data[0] = Lerp1(c1.m_data[0], c2.m_data[0], t);
	out.m_data[1] = Lerp1(c1.m_data[1], c2.m_data[1], t);
	out.m_data[2] = Lerp1(c1.m_data[2], c2.m_data[2], t);
	out.m_data[3] = Lerp1(c1.m_data[3], c2.m_data[3], t);
}

float32_t alMath::Lerp2(float32_t a, float32_t b, float32_t t)
{
	return a + t * (b - a);
}

float64_t alMath::Lerp2(float64_t a, float64_t b, float64_t t)
{
	return a + t * (b - a);
}

// assimp
alQuaternion alMath::Slerp(const alQuaternion& pStart, const alQuaternion& pEnd, const float32_t pFactor)
{
	alQuaternion pOut;
	float32_t cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

	// adjust signs (if necessary)
	alQuaternion end = pEnd;
	if (cosom < 0.0f)
	{
		cosom = -cosom;
		end.x = -end.x;   // Reverse all signs
		end.y = -end.y;
		end.z = -end.z;
		end.w = -end.w;
	}

	// Calculate coefficients
	float32_t sclp, sclq;
	if ((1.0f - cosom) > 0.0001f) // 0.0001 -> some epsillon
	{
		// Standard case (slerp)
		float32_t omega, sinom;
		omega = std::acos(cosom); // extract theta from dot product's cos theta
		sinom = std::sin(omega);
		sclp = std::sin((1.0f - pFactor) * omega) / sinom;
		sclq = std::sin(pFactor * omega) / sinom;
	}
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = 1.0f - pFactor;
		sclq = pFactor;
	}

	pOut.x = sclp * pStart.x + sclq * end.x;
	pOut.y = sclp * pStart.y + sclq * end.y;
	pOut.z = sclp * pStart.z + sclq * end.z;
	pOut.w = sclp * pStart.w + sclq * end.w;
	return pOut;
}

float32_t alMath::Normalize(float32_t max, float32_t val)
{
	return val * (1.f / max);
}

alVec2i alMath::WorldToScreen(const alMat4& VP, const alVec3& point3D, const alVec2f& viewportSize, const alVec2f& offset)
{
	alVec4 point;
	point.x = point3D.x;
	point.y = point3D.y;
	point.z = point3D.z;
	point.w = 1.f;

	point = alMath::Mul(point, VP);

	return alVec2i
	(
		int32_t(offset.x + (viewportSize.x * 0.5f + point.x * viewportSize.x * 0.5f / point.w))
		,
		int32_t(offset.y + (viewportSize.y - (viewportSize.y * 0.5f + point.y * viewportSize.y * 0.5f / point.w)))
	);
}

alVec2i alMath::ScreenToClient(const alVec2i& screen_coord, const alVec4i& client_rect)
{
	return alVec2i(
		screen_coord.x - client_rect.x,
		screen_coord.y - client_rect.y
	);
}

//void alMath::PerspectiveLHMatrix(alMat4& out, float32_t FOV, float32_t aspect, float32_t Near, float32_t Far)
//{
//	float32_t S = std::sin(0.5f * FOV);
//	float32_t C = std::cos(0.5f * FOV);
//	float32_t H = C / S;
//	float32_t W = H / aspect;
//	out[0] = alVec4(W, 0.f, 0.f, 0.f);
//	out[1] = alVec4(0.f, H, 0.f, 0.f);
//	out[2] = alVec4(0.f, 0.f, Far / (Far - Near), 1.f);
//	out[3] = alVec4(0.f, 0.f, -out[2].z * Near, 0.f);
//}

void alMath::PerspectiveRHMatrix(alMat4& out, float32_t FOV, float32_t aspect, float32_t Near, float32_t Far)
{
	float32_t S = std::sin(0.5f * FOV);
	float32_t C = std::cos(0.5f * FOV);
	float32_t H = C / S;
	float32_t W = H / aspect;
	out[0] = alVec4(W, 0.f, 0.f, 0.f);
	out[1] = alVec4(0.f, H, 0.f, 0.f);
	out[2] = alVec4(0.f, 0.f, Far / (Near - Far), -1.f);
	out[3] = alVec4(0.f, 0.f, out[2].z * Near, 0.f);
}

void alMath::OrthoRHMatrix(alMat4& out, float32_t width, float32_t height, float32_t Near, float32_t Far)
{
	out[0] = alVec4(2.f / width, 0.f, 0.f, 0.f);
	out[1] = alVec4(0.f, 2.f / height, 0.f, 0.f);
	out[2] = alVec4(0.f, 0.f, 1.f / (Near - Far), 0.f);
	out[3] = alVec4(0.f, 0.f, out[2].z * Near, 1.f);
}

void alMath::OrthographicOffCenterRHMatrix(alMat4& out, float32_t ViewLeft, float32_t ViewRight, float32_t ViewBottom, float32_t ViewTop, float32_t NearZ, float32_t FarZ)
{
	float32_t ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
	float32_t ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

	out.m_data[0].Set(ReciprocalWidth + ReciprocalWidth, 0.0f, 0.0f, 0.0f);
	out.m_data[1].Set(0.0f, ReciprocalHeight + ReciprocalHeight, 0.0f, 0.0f);
	out.m_data[2].Set(0.0f, 0.0f, 1.0f / (NearZ - FarZ), 0.0f);
	out.m_data[3].Set(-(ViewLeft + ViewRight) * ReciprocalWidth,
		-(ViewTop + ViewBottom) * ReciprocalHeight,
		out.m_data[2].z * NearZ,
		1.0f);
}

void alMath::LookAtRHMatrix(alMat4& out, const alVec3& eye, const alVec3& center, const alVec3& up)
{
	alVec3 f(center - eye);
	f.Normalize2();

	alVec3 s;
	f.Cross2(up, s);
	s.Normalize2();

	alVec3 u;
	s.Cross2(f, u);

	out.Identity();

	out[0].x = s.x;
	out[1].x = s.y;
	out[2].x = s.z;
	out[0].y = u.x;
	out[1].y = u.y;
	out[2].y = u.z;
	out[0].z = -f.x;
	out[1].z = -f.y;
	out[2].z = -f.z;
	out[3].x = -s.Dot(eye);
	out[3].y = -u.Dot(eye);
	out[3].z = f.Dot(eye);
}

void alMath::TranslationMatrix(alMat4& out, const alVec3& position)
{
	out[3].x = position.x;
	out[3].y = position.y;
	out[3].z = position.z;
}

void alMath::ScaleMatrix(alMat4& out, const alVec3& scale)
{
	out[0].x = scale.x;
	out[1].y = scale.y;
	out[2].z = scale.z;
}

alVec4 alMath::Mul(const alVec4& vec, const alMat4& mat)
{
	return alVec4
	(
		mat[0].x * vec.x + mat[1].x * vec.y + mat[2].x * vec.z + mat[3].x * vec.w,
		mat[0].y * vec.x + mat[1].y * vec.y + mat[2].y * vec.z + mat[3].y * vec.w,
		mat[0].z * vec.x + mat[1].z * vec.y + mat[2].z * vec.z + mat[3].z * vec.w,
		mat[0].w * vec.x + mat[1].w * vec.y + mat[2].w * vec.z + mat[3].w * vec.w
	);
}

alVec3 alMath::Mul(const alVec3& vec, const alMat4& mat)
{
	return alVec3
	(
		mat[0].x * vec.x + mat[1].x * vec.y + mat[2].x * vec.z,
		mat[0].y * vec.x + mat[1].y * vec.y + mat[2].y * vec.z,
		mat[0].z * vec.x + mat[1].z * vec.y + mat[2].z * vec.z
	);
}

alVec3f alMath::Mul(const alVec3f& vec, const alMat4& mat)
{
	return alVec3f
	(
		(float32_t)mat[0].x * vec.x + (float32_t)mat[1].x * vec.y + (float32_t)mat[2].x * vec.z + (float32_t)mat[3].x,
		(float32_t)mat[0].y * vec.x + (float32_t)mat[1].y * vec.y + (float32_t)mat[2].y * vec.z + (float32_t)mat[3].y,
		(float32_t)mat[0].z * vec.x + (float32_t)mat[1].z * vec.y + (float32_t)mat[2].z * vec.z + (float32_t)mat[3].z
	);
}

alVec3f alMath::Mul(const alVec3f& vec, const alMat3& mat)
{
	return alVec3f
	(
		(float32_t)mat[0].x * vec.x + (float32_t)mat[1].x * vec.y + (float32_t)mat[2].x * vec.z + (float32_t)mat[3].x,
		(float32_t)mat[0].y * vec.x + (float32_t)mat[1].y * vec.y + (float32_t)mat[2].y * vec.z + (float32_t)mat[3].y,
		(float32_t)mat[0].z * vec.x + (float32_t)mat[1].z * vec.y + (float32_t)mat[2].z * vec.z + (float32_t)mat[3].z
	);
}

alVec3f alMath::MulBasis(const alVec3f& vec, const alMat4& mat)
{
	return alVec3f
	(
		(float32_t)mat[0].x * vec.x + (float32_t)mat[1].x * vec.y + (float32_t)mat[2].x * vec.z,
		(float32_t)mat[0].y * vec.x + (float32_t)mat[1].y * vec.y + (float32_t)mat[2].y * vec.z,
		(float32_t)mat[0].z * vec.x + (float32_t)mat[1].z * vec.y + (float32_t)mat[2].z * vec.z
	);
}

alQuaternion alMath::MatToQuat(const alMat4& mat)
{
	alQuaternion q;
	float32_t trace = float32_t(mat[0].x + mat[1].y + mat[2].z);
	if (trace > 0.0f)
	{

		float32_t t = trace + 1.0f;
		float32_t s = std::sqrt(t) * 0.5f;

		q.w = s * t;
		q.x = float32_t(mat[2].y - mat[1].z) * s;
		q.y = float32_t(mat[0].z - mat[2].x) * s;
		q.z = float32_t(mat[1].x - mat[0].y) * s;

	}
	else
	{
		static int32_t	next[3] = { 1, 2, 0 };
		int32_t i = 0;
		if (mat[1][1] > mat[0][0])
			i = 1;
		if (mat[2][2] > mat[i][i])
			i = 2;
		int32_t j = next[i];
		int32_t k = next[j];

		float32_t t = float32_t(mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0f;
		float32_t s = std::sqrt(t) * 0.5f;

		q[i] = s * t;
		q[3] = float32_t(mat[k][j] - mat[j][k]) * s;
		q[j] = float32_t(mat[j][i] + mat[i][j]) * s;
		q[k] = float32_t(mat[k][i] + mat[i][k]) * s;
	}
	return q;
}



