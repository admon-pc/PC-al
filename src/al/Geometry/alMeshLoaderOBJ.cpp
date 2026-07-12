#include "al.h"
#include "Geometry/alMeshLoader.h"
#include "Geometry/alPopygonMesh.h"

#include "alMeshLoaderOBJ.h"
#include <vector>
#include <unordered_map>

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

enum class OBJFaceType
{
	p,
	pu,
	pun,
	pn
};

struct OBJSimpleArr
{
	OBJSimpleArr() {
		sz = 0;
	}

	int data[0x100];
	unsigned int sz;

	void push_back(int v) { data[sz++] = v; }
	unsigned int size() { return sz; }
	void reset() { sz = 0; }
};

struct OBJFace
{
	OBJFace() {
		ft = OBJFaceType::pun;
	}

	OBJSimpleArr p, u, n;
	OBJFaceType ft;

	void reset()
	{
		ft = OBJFaceType::pun;
		p.reset();
		u.reset();
		n.reset();
	}
};

unsigned char* OBJNextLine(unsigned char* ptr);
unsigned char* OBJSkipSpaces(unsigned char* ptr);
unsigned char* OBJReadVec2(unsigned char* ptr, alVec2f& vec2);
unsigned char* OBJReadFloat(unsigned char* ptr, float& value);
unsigned char* OBJReadVec3(unsigned char* ptr, alVec3f& vec3);
unsigned char* OBJReadFace(unsigned char* ptr, OBJFace& f, char* s);
unsigned char* OBJReadWord(unsigned char* ptr, alStringW& str);
unsigned char* OBJReadLastWord(unsigned char* ptr, alStringW& str);


alMeshLoaderOBJ::alMeshLoaderOBJ()
{
}

alMeshLoaderOBJ::~alMeshLoaderOBJ()
{
}

bool alMeshLoaderOBJ::Load(const char* fn, alMeshLoaderCallback* cb)
{
	AL_ASSERT_ST(fn);

	alFileBuffer fb;
	if (fb.ReadFile(fn))
	{
		if (Load(&fb, cb))
			return true;
	}
	else
	{
		alLog::PrintWarning("OBJ: failed to open file\n");
	}
	alLog::PrintWarning("OBJ: file [%s]\n", fn);

	return false;
}

bool alMeshLoaderOBJ::Load(alFileBuffer* fb, alMeshLoaderCallback* cb)
{
	auto file_size = (size_t)fb->Size();

	std::vector<unsigned char> file_byte_array((unsigned int)file_size + 2);

	unsigned char* ptr = file_byte_array.data();
	memcpy(ptr, fb->Data(), file_size);

	ptr[(unsigned int)file_size] = ' ';
	ptr[(unsigned int)file_size + 1] = 0;

	bool groupBegin = false;
	bool isModel = false;
	bool grpFound = false;

	alVec2f tcoords;
	alVec3f pos;
	alVec3f norm;

	std::vector<alVec3f> position;
	std::vector<alVec2f> uv;
	std::vector<alVec3f> normal;

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	//std::string name_word;
	alStringW tmp_word;
	alStringW curr_word;
	alStringW prev_word;

	OBJFace f;
	char s[0xff];

	int last_counter[3] = { 0,0,0 };

	std::unordered_map<std::string, unsigned int> map;
	std::string hash;

	alPolygonCreator polygonCreator;
	alPolygonMesh polygonMesh;

	while (*ptr)
	{
		switch (*ptr)
		{
		case 'm'://mtllib
		case 'u'://usemtl
		case 's':
		case 'l':
		case 'c'://curv
		case 'p'://parm
		case 'd'://deg
		case '#':
		case 'e'://end
			ptr = OBJNextLine(ptr);
			break;
		case 'v':
		{
			++ptr;
			if (groupBegin)
				groupBegin = false;
			switch (*ptr)
			{
			case 't':
				ptr = OBJReadVec2(++ptr, tcoords);
				uv.push_back(tcoords);
				++last_counter[1];
				break;
			case 'n':
				ptr = OBJReadVec3(++ptr, norm);
				normal.push_back(norm);
				++last_counter[2];
				break;
			default:
				ptr = OBJReadVec3(ptr, pos);
				position.push_back(pos);
				++last_counter[0];
				//newModel->m_aabb.add(pos);
				break;
			}
		}break;
		case 'f':
		{
			isModel = true;
			f.reset();
			ptr = OBJReadFace(++ptr, f, s);

			polygonCreator.Clear();

			bool weld = false;
			bool genNormals = true;

			for (size_t sz2 = f.p.size(), i2 = 0; i2 < sz2; ++i2)
			{
				auto index = i2;
				auto pos_index = f.p.data[index];


				if (pos_index < 0)
				{
					// если индекс отрицательный то он указывает на позицию относительно последнего элемента
					// -1 = последний элемент
					pos_index = last_counter[0] + pos_index + 1;
				}

				{
					hash.clear();
					hash += pos_index;

					// это я не помню зачем сделал
					// когда дойду до control вершин, станет ясно зачем это здесь
					auto it = map.find(hash);
					if (it == map.end())
					{
						map[hash] = pos_index;
					}
					else
					{
						weld = true;
					}
				}

				auto v = position[pos_index];

				//geometry_creator->AddPosition(v.x, v.y, v.z);
				alVec3f pcPos, pcNorm;
				alVec2f pcUV;

				pcPos = v;

				if (f.ft == OBJFaceType::pu || f.ft == OBJFaceType::pun)
				{
					auto uv_index = f.u.data[index];

					if (uv_index < 0)
					{
						uv_index = last_counter[1] + uv_index + 1;
					}

					auto u = uv[uv_index];
					//	geometry_creator->AddUV(u.x, u.y);
					pcUV.x = u.x;
					pcUV.y = 1.f - u.y;
				}

				if (f.ft == OBJFaceType::pn || f.ft == OBJFaceType::pun)
				{
					auto nor_index = f.n.data[index];

					if (nor_index < 0)
					{
						nor_index = last_counter[2] + nor_index + 1;
					}

					auto n = normal[nor_index];
					//	geometry_creator->AddNormal(n.x, n.y, n.z);
					pcNorm = n;
					genNormals = false;
				}
				
				//importerHelper.m_polygonCreator.Add(pcPos, weld, false, pcNorm, pcUV);
				polygonCreator.SetPosition(pcPos);
				polygonCreator.SetNormal(pcNorm);
				polygonCreator.SetUV(pcUV);
				polygonCreator.Add(weld);
			}

			/*if (g_ImportOBJ_triangulate && importerHelper.m_polygonCreator.Size() > 3)
			{
				u32 triCount = importerHelper.m_polygonCreator.Size() - 2;
				auto _positions = importerHelper.m_polygonCreator.GetPositions();
				auto _normals = importerHelper.m_polygonCreator.GetNormals();
				auto _tcoords = importerHelper.m_polygonCreator.GetTCoords();
				for (u32 i = 0; i < triCount; ++i)
				{
					triangulationPolygonCreator.Clear();
					triangulationPolygonCreator.Add(_positions[0].m_first, weld, false, _normals[0], _tcoords[0]);
					triangulationPolygonCreator.Add(_positions[i + 1].m_first, weld, false, _normals[i + 1], _tcoords[i + 1]);
					triangulationPolygonCreator.Add(_positions[i + 2].m_first, weld, false, _normals[i + 2], _tcoords[i + 2]);

					importerHelper.m_meshBuilder->AddPolygon(&triangulationPolygonCreator, genNormals);
				}
			}
			else*/
			//{
				//importerHelper.m_meshBuilder->AddPolygon(&importerHelper.m_polygonCreator, genNormals);
			polygonMesh.AddPolygon(&polygonCreator);
			//}
		}break;
		case 'o':
		case 'g':
		{
			if (!groupBegin)
				groupBegin = true;
			else
			{
				ptr = OBJNextLine(ptr);
				break;
			}

			/*std::string tmp_word;
			ptr = OBJReadWord(++ptr, tmp_word);
			if (tmp_word.size())
			{
				if (!name_word.size())
					name_word = tmp_word;
			}*/
			ptr = OBJReadWord(++ptr, tmp_word);
			if (tmp_word.size())
			{
				prev_word = curr_word;
				curr_word = tmp_word;
			}

			if (grpFound)
			{
				//if (importerHelper.m_meshBuilder->m_isBegin)
				//{
				//	importerHelper.m_meshBuilder->End();
				//	miMaterial* m = 0;
				//	if (currMaterial)
				//	{
				//		m = g_sdk->CreateMaterial(currMaterial->m_name.data());
				//		if (currMaterial->m_map_diffuse.size())
				//			m->m_maps[m->mapSlot_Diffuse].m_texturePath = currMaterial->m_map_diffuse;
				//	}
				//	g_sdk->CreateSceneObjectFromHelper(&importerHelper, prev_word.data(), m);
				//	// just create again
				//	importerHelper.Create();
				//	importerHelper.m_meshBuilder->Begin();
				//}
				
				//polygonMesh.GenerateNormals(true);
				alMesh* mesh = polygonMesh.CreateMesh();
				if (mesh)
				{
					mesh->m_name = prev_word;
					cb->OnMesh(mesh);
				}
			}
			grpFound = true;
		}break;
		default:
			++ptr;
			break;
		}
	}

	/*if (importerHelper.m_meshBuilder->m_isBegin)
	{
		importerHelper.m_meshBuilder->End();

		miMaterial* m = 0;
		if (currMaterial)
		{
			m = g_sdk->CreateMaterial(currMaterial->m_name.data());
			if (currMaterial->m_map_diffuse.size())
				m->m_maps[m->mapSlot_Diffuse].m_texturePath = currMaterial->m_map_diffuse;
		}

		g_sdk->CreateSceneObjectFromHelper(&importerHelper, curr_word.data(), m);
	}*/
	//polygonMesh.GenerateNormals(true);
	alMesh* mesh = polygonMesh.CreateMesh();
	if (mesh)
	{
		mesh->m_name = curr_word;
		cb->OnMesh(mesh);
	}

	return true;
}

uint32_t alMeshLoaderOBJ::GetExtensionNum()
{
	return 1;
}

const char* alMeshLoaderOBJ::GetExtension(uint32_t)
{
	return "obj";
}

unsigned char* OBJNextLine(unsigned char* ptr)
{
	while (*ptr)
	{
		if (*ptr == '\n')
		{
			ptr++;
			return ptr;
		}
		ptr++;
	}
	return ptr;
}

unsigned char* OBJReadVec2(unsigned char* ptr, alVec2f& vec2)
{
	ptr = OBJSkipSpaces(ptr);
	float x, y;
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJNextLine(ptr);
		vec2.x = x;
		vec2.y = y;
	}
	return ptr;
}

unsigned char* OBJSkipSpaces(unsigned char* ptr)
{
	while (*ptr)
	{
		if (*ptr != '\t' && *ptr != ' ')
			break;
		ptr++;
	}
	return ptr;
}

unsigned char* OBJReadFloat(unsigned char* ptr, float& value)
{
	char str[32u];
	memset(str, 0, 32);
	char* p = &str[0u];
	while (*ptr) {
		if (!isdigit(*ptr) && (*ptr != '-') && (*ptr != '+')
			&& (*ptr != 'e') && (*ptr != 'E') && (*ptr != '.')) break;
		*p = *ptr;
		++p;
		++ptr;
	}
	value = (float)atof(str);
	return ptr;
}

unsigned char* OBJReadVec3(unsigned char* ptr, alVec3f& vec3) {
	ptr = OBJSkipSpaces(ptr);
	float x, y, z;
	if (*ptr == '\n') {
		ptr++;
	}
	else {
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, z);
		ptr = OBJNextLine(ptr);
		vec3.x = x;
		vec3.y = y;
		vec3.z = z;
	}
	return ptr;
}

unsigned char* OBJSkipSpace(unsigned char* ptr) {
	while (*ptr) {
		if (*ptr != ' ' && *ptr != '\t') break;
		ptr++;
	}
	return ptr;
}

unsigned char* OBJGetInt(unsigned char* p, int& i)
{
	char str[8u];
	memset(str, 0, 8);
	char* pi = &str[0u];

	while (*p)
	{
		/*if( *p == '-' )
		{
		++p;
		continue;
		}*/

		if (!isdigit(*p) && *p != '-') break;


		*pi = *p;
		++pi;
		++p;
	}
	i = atoi(str);
	return p;
}

unsigned char* OBJReadFace(unsigned char* ptr, OBJFace& f, char* s) {
	ptr = OBJSkipSpaces(ptr);
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		while (true)
		{
			int p = 1;
			int u = 1;
			int n = 1;

			ptr = OBJGetInt(ptr, p);

			if (*ptr == '/')
			{
				ptr++;
				if (*ptr == '/')
				{
					ptr++;
					f.ft = OBJFaceType::pn;
					ptr = OBJGetInt(ptr, n);
				}
				else
				{
					ptr = OBJGetInt(ptr, u);
					if (*ptr == '/')
					{
						ptr++;
						f.ft = OBJFaceType::pun;
						ptr = OBJGetInt(ptr, n);
					}
					else
					{
						f.ft = OBJFaceType::pu;
					}
				}
			}
			else
			{
				f.ft = OBJFaceType::p;
			}
			f.n.push_back(n - 1);
			f.u.push_back(u - 1);
			f.p.push_back(p - 1);
			ptr = OBJSkipSpace(ptr);

			if (*ptr == '\r')
				break;
			else if (*ptr == '\n')
				break;
		}
	}
	return ptr;
}

unsigned char* OBJReadWord(unsigned char* ptr, alStringW& str)
{
	ptr = OBJSkipSpaces(ptr);
	str.clear();
	while (*ptr)
	{
		if (isspace(*ptr))
			break;
		str += (wchar_t)*ptr;
		ptr++;
	}
	return ptr;
}

unsigned char* OBJReadLastWord(unsigned char* ptr, alStringW& str) {
	while (true)
	{
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadWord(ptr, str);

		ptr = OBJSkipSpaces(ptr);

		if (*ptr == '\r' || *ptr == '\n')
		{
			break;
		}

		if (*ptr == 0)
			break;
	}
	return ptr;
}