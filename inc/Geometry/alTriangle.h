#ifndef _AL_TRIANGLEH_
#define _AL_TRIANGLEH_

class alTriangle
{
public:
	alTriangle();
	alTriangle(const alVec4f& _v1, const alVec4f& _v2, const alVec4f& _v3);
	alTriangle(const alVec3f& _v1, const alVec3f& _v2, const alVec3f& _v3);
	
	alVec4f v1;
	alVec4f v2;
	alVec4f v3;
	//alVec4f faceNormal;
	alVec4f normal1;
	alVec4f normal2;
	alVec4f normal3;
	alVec4f e1;
	alVec4f e2;
	alVec4f t1;
	alVec4f t2;
	alVec4f t3;

	void Update();
	void Center(alVec4f& out);
	bool RayTest_MT(const alRay& ray, bool withBackFace, float32_t& T, float& U, float& V, float& W);
	bool RayTest_MT(const alRay& ray, bool withBackFace, float64_t& T, float& U, float& V, float& W);
	bool RayTest_Watertight(const alRay& ray, bool withBackFace, float& T, float& U, float& V, float& W);
};

#endif

