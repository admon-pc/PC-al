#ifndef _AL_MATHH_
#define _AL_MATHH_

#define alInfinity std::numeric_limits<float32_t>::infinity()
#define alEpsilon std::numeric_limits<float32_t>::epsilon()

/*
		   0 : 0
		22.5 : 0.3926991
		  45 : 0.7853982
		67.5 : 1.1780972
		  90 : 1.5707963
	   112.5 : 1.9634954
		 135 : 2.3561945
	   157.5 : 2.7488936
		 180 : 3.1415927
	   202.5 : 3.5342917
		 225 : 3.9269908
	   247.5 : 4.3196899
		 270 : 4.7123890
	   292.5 : 5.1050881
		 315 : 5.4977871
	   337.5 : 5.8904862
		 360 : 0
	*/
constexpr float32_t PIf = static_cast<float32_t>(3.14159265358979323846);
constexpr float32_t PIfHalf = static_cast<float32_t>(3.14159265358979323846 * 0.5);
constexpr float32_t PIfPlusHalf = static_cast<float32_t>(3.14159265358979323846 + PIfHalf);
constexpr float32_t PIPIf = 6.2831853f;
constexpr float64_t PI = 3.14159265358979323846;
constexpr float64_t PIHalf = 3.14159265358979323846 * 0.5;
constexpr float64_t PIPlusHalf = 3.14159265358979323846 + PIHalf;
constexpr float64_t PIPI = PI+PI;


#include "Math/alVector.h"
#include "Math/alQuaternion.h"
#include "Math/alMatrix.h"

const alVec4 alVec4FltMax = alVec4(FLT_MAX);
const alVec4 alVec4FltMaxNeg = alVec4(-FLT_MAX);
const alVec4f alVec4fFltMax = alVec4f(FLT_MAX);
const alVec4f alVec4fFltMaxNeg = alVec4f(-FLT_MAX);

class alMath
{
public:
	static float32_t Distance2d(float32_t p1x, float32_t p1y, float32_t p2x, float32_t p2y);
	static float32_t Distance2d(const alVec2f p1, const alVec2f p2);
	static bool PointInRect(int32_t x, int32_t y, const alVec4i& rect);
	static bool PointInRect(int32_t x, int32_t y, const alVec4f& rect);
	static bool PointInRect(float32_t x, float32_t y, const alVec4f& rect);
	static float32_t DegToRad(float32_t degrees);
	static float32_t RadToDeg(float32_t radians);
	static float32_t Clamp(float32_t x, float32_t min, float32_t max);
	static float32_t Lerp1(float32_t a, float32_t b, float32_t t);
	static float64_t Lerp1(float64_t a, float64_t b, float64_t t);
	static void Lerp1(const alColor& c1, const alColor& c2, float32_t t, alColor& out);
	static float32_t Lerp2(float32_t a, float32_t b, float32_t t);
	static float64_t Lerp2(float64_t a, float64_t b, float64_t t);
	static alQuaternion Slerp(const alQuaternion& pStart, const alQuaternion& pEnd, const float32_t pFactor);
	static float32_t Normalize(float32_t max, float32_t val);
	static alVec2i WorldToScreen(const alMat4& VP, const alVec3& point3D, const alVec2f& viewportSize, const alVec2f& offset);
	static alVec2i ScreenToClient(const alVec2i& screen_coord, const alVec4i& client_rect);
//	static void PerspectiveLHMatrix(alMat4& out, float32_t FOV, float32_t aspect, float32_t Near, float32_t Far);
	static void PerspectiveRHMatrix(alMat4& out, float32_t FOV, float32_t aspect, float32_t Near, float32_t Far);
	static void OrthoRHMatrix(alMat4& out, float32_t width, float32_t height, float32_t Near, float32_t Far);
	static void OrthographicOffCenterRHMatrix(alMat4& out, float32_t ViewLeft, float32_t ViewRight, float32_t ViewBottom, float32_t ViewTop, float32_t NearZ, float32_t FarZ);
	static void LookAtRHMatrix(alMat4& out, const alVec3& eye, const alVec3& center, const alVec3& up = alVec3(0.f, 1.f, 0.f));
	static void TranslationMatrix(alMat4& out, const alVec3& position);
	static void ScaleMatrix(alMat4& out, const alVec3& scale );
	static alVec4 Mul(const alVec4& vec, const alMat4& mat);
	static alVec3 Mul(const alVec3& vec, const alMat4& mat);
	static alVec3f Mul(const alVec3f& vec, const alMat4& mat);
	static alVec3f Mul(const alVec3f& vec, const alMat3& mat);
	static alVec3f MulBasis(const alVec3f& vec, const alMat4& mat);
	static alQuaternion MatToQuat(const alMat4& mat);
};

#endif

