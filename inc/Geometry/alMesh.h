#ifndef _AL_MESHH_
#define _AL_MESHH_

struct alVertexGUI
{
	alVec2f Position;
	alVec2f UV;
};

struct alVertexPoint
{
	alVec3f Position;
	alVec4f Color;
};

struct alVertexAnimatedPoint
{
	alVec3f Position;
	alVec4f Color;
	alVec4f Weights; // потом надо переделать на байты чтобы уменьшить вес
	alVec4i Bones;
};

struct alVertexTriangle
{
	alVec3f Position;
	alVec2f UV1;
	alVec2f UV2;
	alVec3f Normal;
	alVec3f Binormal;
	alVec3f Tangent;
	alVec4f Color;
};

struct alVertexAnimatedTriangle
{
	alVec3f Position;
	alVec2f UV1;
	alVec2f UV2;
	alVec3f Normal;
	alVec3f Binormal;
	alVec3f Tangent;
	alVec4f Color;

	alVec4f Weights; // потом надо переделать на байты чтобы уменьшить вес
	alVec4i Bones;
};

struct alVertexLine
{
	alVec3f Position;
	alVec3f Normal;
	alVec4f Color;
};

struct alVertexAnimatedLine
{
	alVec3f Position;
	alVec3f Normal;
	alVec4f Color;

	alVec4f Weights; // потом надо переделать на байты чтобы уменьшить вес
	alVec4i Bones;
};

// тип
// ещё надо будет добавить для анимированных моделей
enum class alMeshVertexType : uint32_t
{
	Null, // 0
	GUI,
	Triangle,
	AnimatedTriangle,
	Line,
	AnimatedLine,
	Point,
	AnimatedPoint,
};

// предпологается что при создании MDL индекс будет устанавливаться автоматически, в зависимости от количества треугольников 
//  пока не протестировано. условие такое if (modelInds.size() / 3 > 21845) newModel->m_indexType = yyMeshIndexType::uint32_t; 
enum class alMeshIndexType : uint32_t
{
	u16,
	u32
};

//struct miJoint
//{
//	miJoint()
//	{
//		m_parentIndex = -1;
//	}
//
//	int32_t m_parentIndex; // index in yyMDL::m_joints
//
//	Mat4					m_matrixBindInverse;
//	Mat4					m_matrixOffset;
//	Mat4					m_matrixWorld;
//
//	miStringA				m_name;
//
//};
//
//#define MI_MDL_VERSION 1
//
//struct miMDLHeader
//{
//	miMDLHeader(){}
//
//	uint32_t m_numOfLayers = 0;
//	uint32_t m_numOfJoints = 0;
//	uint32_t m_numOfAnimations = 0;
//	uint32_t m_numOfHitboxes = 0;
//
//	uint32_t m_stringsOffset = 0;
//
//	alVec3f m_aabbMin;
//	alVec3f m_aabbMax;
//
//
//	Mat4 m_preRotation;
//};
//struct miMDLLayerHeader
//{
//	miMDLLayerHeader()
//	{
//		for (int32_t i = 0; i < YY_MDL_LAYER_NUM_OF_TEXTURES; ++i)
//		{
//			m_textureStrID[i] = -1;
//		}
//	}
//	// 0 - simple
//	uint32_t m_shaderType = 0;
//	int32_t m_textureStrID[YY_MDL_LAYER_NUM_OF_TEXTURES];
//	uint32_t m_vertexCount = 0;
//	// 0 - yyVertexModel
//	// 1 - yyVertexAnimatedModel
//	uint32_t m_vertexType = 0;
//	uint32_t m_vertexDataSize = 0;
//	uint32_t m_indexCount = 0;
//	// 0 - 16bit
//	// 1 - 32bit
//	uint32_t m_indexType = 0;
//	uint32_t m_indexDataSize = 0;
//};
//struct miMDLJointHeader
//{
//	miMDLJointHeader(){}
//	int32_t m_nameStrID = -1;
//	int32_t m_parentID = -1;
//	Mat4					m_matrixBindInverse;
//	Mat4					m_matrixOffset;
//	Mat4					m_matrixWorld;
//};
//struct miMDLAnimationHeader
//{
//	miMDLAnimationHeader(){}
//	int32_t m_nameStrID = -1;
//	float32_t m_length = 0.f;
//	float32_t m_fps = 0.f;
//	uint32_t m_numOfAnimatedJoints = 0;
//	uint32_t m_flags = 0;
//};
//struct miMDLAnimatedJointHeader
//{
//	miMDLAnimatedJointHeader()
//	{
//		m_jointID = m_numOfKeyFrames;
//	}
//	int32_t m_jointID = -1;
//	uint32_t m_numOfKeyFrames;
//};
//struct miMDLJointKeyframeHeader
//{
//	miMDLJointKeyframeHeader()
//	{
//		m_scale.set(1.f);
//	}
//	int32_t m_time = 0;
//	alVec3f m_position;
//	alVec3f m_scale;
//	Quat m_rotation;
//};
//
//struct miMDLHitboxHeader
//{
//	miMDLHitboxHeader()	{	}
//	uint32_t m_type = 0;
//	int32_t m_jointID = -1;
//	uint32_t m_vertexCount = 0;
//	uint32_t m_indexCount = 0;
//};

// описание одного буфера для рисования
class alMesh
{
public:
	alMesh();
	~alMesh();

	void PrintAllInfo(bool onlyPosition);

	alAabb m_aabb;

	uint8_t* m_vertices = nullptr;
	uint8_t* m_indices = nullptr;
	alMeshIndexType m_indexType = alMeshIndexType::u16;

	uint32_t m_vCount = 0;
	uint32_t m_iCount = 0;
	uint32_t m_stride = 0;

	alMeshVertexType m_vertexType = alMeshVertexType::Triangle;

	alStringW m_name;

	// IrrLicht
	void CalculateTangents(alVec3f& normal, alVec3f& tangent, alVec3f& binormal,
		const alVec3f& vt1, const alVec3f& vt2, const alVec3f& vt3, // vertices
		const alVec2f& tc1, const alVec2f& tc2, const alVec2f& tc3); // texture coords
	void GenerateTangents_u16();
	void GenerateTangents_u32();
	void GenerateTangents();
};

#endif

