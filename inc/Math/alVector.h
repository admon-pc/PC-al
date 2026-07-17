#ifndef _AL_MATHVECTOR_H_
#define _AL_MATHVECTOR_H_

template<typename Type>
class alVec4_t;

template<typename Type>
class alVec2_t
{
public:
	alVec2_t<Type>(){}
	alVec2_t<Type>(Type _x, Type _y) :x(_x), y(_y) {}
	alVec2_t<Type>(Type val) :x(val), y(val) {}
	Type x = 0;
	Type y = 0;

	void Set(Type _x, Type _y) { x = _x; y = _y; }
	alVec2_t<Type> operator*(const alVec2_t<Type>& v)const { alVec2_t<Type> r; r.x = x * v.x; r.y = y * v.y; return r; }
	alVec2_t<Type> operator-(const alVec2_t<Type>& v)const { alVec2_t<Type> r; r.x = x - v.x; r.y = y - v.y; return r; }
	alVec2_t<Type> operator+(const alVec2_t<Type>& v)const { alVec2_t<Type> r; r.x = x + v.x; r.y = y + v.y; return r; }
	alVec2_t<Type> operator/(const alVec2_t<Type>& v)const { alVec2_t<Type> r; r.x = x / v.x; r.y = y / v.y; return r; }

	void operator+=(const alVec2_t<Type>& v) { x += v.x; y += v.y; }
	void operator-=(const alVec2_t<Type>& v) { x -= v.x; y -= v.y; }
	void operator*=(const alVec2_t<Type>& v) { x *= v.x; y *= v.y; }
	void operator/=(const alVec2_t<Type>& v) { x /= v.x; y /= v.y; }
	bool operator==(const alVec2_t<Type>& v)const { if (x != v.x)return false; if (y != v.y)return false; return true; }
	Type Distance(const alVec2_t<Type>& from)const { return alVec2_t<Type>(x - from.x, y - from.y).Length(); }
	Type Length()const { return std::sqrt(LengthSqrt()); }
	Type LengthSqrt()const { return(x * x) + (y * y); }
	Type	Dot()const { return (x * x) + (y * y); }
	void	Normalize2()
	{
		Type len = std::sqrt(Dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len;
	}
	alVec2_t<Type> operator*(Type v)const { alVec2_t<Type> r; r.x = x * v; r.y = y * v; return r; }
};

template<typename Type>
class alVec3_t
{
public:
	alVec3_t<Type>(){}
	alVec3_t<Type>(const alVec4_t<Type>& v);// : x(v.x), y(v.y), z(v.z) {}
	alVec3_t<Type>(Type _v) :x(_v), y(_v), z(_v) {}
	alVec3_t<Type>(Type _x, Type _y, Type _z) :x(_x), y(_y), z(_z) {}
	Type x = 0;
	Type y = 0;
	Type z = 0;

	void Set(Type _x, Type _y, Type _z) { x = _x; y = _y; z = _z; }
	void Set(Type val) { x = y = z = val; }
	Type* data() { return &x; }
	Type Distance(const alVec3_t<Type>& from)const { return alVec3_t<Type>(x - from.x, y - from.y, z - from.z).Length(); }
	//Type Distance(const alVec4_t<Type>& from)const { return alVec3_t<Type>(x - from.x, y - from.y, z - from.z).Length(); }
	void operator+=(const alVec3_t<Type>& v) { x += v.x; y += v.y; z += v.z; }
	void operator-=(const alVec3_t<Type>& v) { x -= v.x; y -= v.y; z -= v.z; }
	void operator*=(const alVec3_t<Type>& v) { x *= v.x; y *= v.y; z *= v.z; }
	void operator/=(const alVec3_t<Type>& v) { x /= v.x; y /= v.y; z /= v.z; }
	Type operator[](int32_t index)const { AL_ASSERT_ST((index >= 0) && (index < 3)); return (&x)[index]; }
	Type& operator[](int32_t index) { AL_ASSERT_ST((index >= 0) && (index < 3)); return (&x)[index]; }
	alVec3_t<Type> operator+(const alVec3_t<Type>& v)const { alVec3_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
	alVec3_t<Type> operator-(const alVec3_t<Type>& v)const { alVec3_t<Type> r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; return r; }
	alVec3_t<Type> operator*(const alVec3_t<Type>& v)const { alVec3_t<Type> r; r.x = x * v.x; r.y = y * v.y; r.z = z * v.z; return r; }
	alVec3_t<Type> operator/(const alVec3_t<Type>& v)const { alVec3_t<Type> r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; return r; }
	//alVec3_t<Type> operator+(const alVec4_t<Type>& v)const { alVec3_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
	//alVec3_t<Type>& operator=(const alVec4_t<Type>& v) { x = v.x; y = v.y; z = v.z; return *this; }
	//void operator+=(const alVec4_t<Type>& v) { x += v.x; y += v.y; z += v.z; }
	bool operator==(const alVec3_t<Type>& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		if (z != v.z)return false;
		return true;
	}
	bool operator!=(const alVec3_t<Type>& v)const {
		if (x != v.x)return true;
		if (y != v.y)return true;
		if (z != v.z)return true;
		return false;
	}
	alVec3_t<Type> Cross(const alVec3_t<Type>& a)const {
		alVec3_t<Type> out;
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
		return out;
	}
	void Cross2(const alVec3_t<Type>& a, alVec3_t<Type>& out)const {
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	void	Normalize2()
	{
		Type len = std::sqrt(Dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len; z *= len;
	}
	Type	Dot(const alVec3_t<Type>& V2)const { return (x * V2.x) + (y * V2.y) + (z * V2.z); }
	Type	Dot()const { return (x * x) + (y * y) + (z * z); }
	alVec3_t<Type> operator-()const { alVec3_t<Type> r; r.x = -x; r.y = -y; r.z = -z; return r; }
	Type Length() const { return std::sqrt(Length2()); }
	Type Length2() const { return Dot(*this); }

	//void add(const alVec4_t<Type>&);
};

template<typename Type>
class alVec4_t
{
public:
	alVec4_t<Type>() {}
	alVec4_t<Type>(Type _v) :x(_v), y(_v), z(_v), w(_v) {}
	alVec4_t<Type>(Type _x, Type _y, Type _z, Type _w) : x(_x), y(_y), z(_z), w(_w) {}
	alVec4_t<Type>(const alVec3_t<float32_t>& _v) : x(_v.x), y(_v.y), z(_v.z), w(0) {}
	Type x = 0;
	Type y = 0;
	Type z = 0;
	Type w = 0;

	Type* Data() { return &x; }
	Type operator[](int32_t index)const { AL_ASSERT_ST((index >= 0) && (index < 4)); return (&x)[index]; }
	Type& operator[](int32_t index) { AL_ASSERT_ST((index >= 0) && (index < 4)); return (&x)[index]; }

	void Set(Type _x, Type _y, Type _z, Type _w) { x = _x; y = _y; z = _z; w = _w; }
	void Set(Type val) { x = y = z = w = val; }
	void operator+=(const alVec4_t<Type>& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
	void operator-=(const alVec4_t<Type>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	void operator*=(const alVec4_t<Type>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
	void operator/=(const alVec4_t<Type>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }

	alVec4_t<Type> operator+(const alVec4_t<Type>& v)const { alVec4_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; r.w = w + v.w; return r; }
	alVec4_t<Type> operator-(const alVec4_t<Type>& v)const { alVec4_t<Type> r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; r.w = w - v.w; return r; }
	alVec4_t<Type> operator*(const alVec4_t<Type>& v)const { alVec4_t<Type> r; r.x = x * v.x; r.y = y * v.y; r.z = z * v.z; r.w = w * v.w; return r; }
	alVec4_t<Type> operator/(const alVec4_t<Type>& v)const { alVec4_t<Type> r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; r.w = w / v.w; return r; }

	alVec4_t<Type> operator-()const { alVec4_t<Type> r; r.x = -x; r.y = -y; r.z = -z; r.w = -w; return r; }

	//alVec4_t<Type> operator+(const alVec4_t<float64_t>& v)const { alVec4_t<Type> r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; r.w = w + v.w; return r; }

	Type	Dot(const alVec4_t<Type>& V2)const { return (x * V2.x) + (y * V2.y) + (z * V2.z) + (w * V2.w); }
	Type	Dot()const { return (x * x) + (y * y) + (z * z) + (w * w); }
	void	Normalize2()
	{
		Type len = std::sqrt(Dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len; z *= len; w *= len;
	}
	void Cross2(const alVec4_t<Type>& a, alVec4_t<Type>& out)const {
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	void Cross2(const alVec4_t<Type>& a, alVec3_t<Type>& out)const {
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	Type LengthSqrt()const { return(x * x) + (y * y) + (z * z); }
	Type Length()const { return std::sqrt(LengthSqrt()); }
	Type Distance(const alVec4_t<Type>& from)const { return alVec4_t<Type>(x - from.x, y - from.y, z - from.z, 1.f).Length(); }
	bool operator==(const alVec4_t<Type>& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		if (z != v.z)return false;
		if (w != v.w)return false;
		return true;
	}
};
AL_FORCE_INLINE alVec4_t<float32_t> operator*(const float32_t& s, const alVec4_t<float32_t>& v) { return v * s; }
AL_FORCE_INLINE alVec4_t<float64_t> operator*(const float64_t& s, const alVec4_t<float64_t>& v) { return v * s; }
AL_FORCE_INLINE alVec4_t<float64_t> operator+(const alVec4_t<float64_t>& s, const alVec4_t<float32_t>& v) 
{
	alVec4_t<float64_t> r;
	r.x = s.x + v.x;
	r.y = s.y + v.y;
	r.z = s.z + v.z;
	r.w = s.w + v.w;
	return r; 
}
AL_FORCE_INLINE alVec4_t<float32_t> operator-(const alVec4_t<float32_t>& s, const alVec4_t<float64_t>& v)
{
	alVec4_t<float32_t> r;
	r.x = s.x - (float32_t)v.x;
	r.y = s.y - (float32_t)v.y;
	r.z = s.z - (float32_t)v.z;
	r.w = s.w - (float32_t)v.w;
	return r;
}

template<typename T>
alVec3_t<T>::alVec3_t(const alVec4_t<T>& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

using alVec2f = alVec2_t<float32_t>;
using alVec2i = alVec2_t<int32_t>;
using alVec2u = alVec2_t<uint32_t>;
using alVec3f = alVec3_t<float32_t>;
using alVec3i = alVec3_t<int32_t>;
using alVec3u = alVec3_t<uint32_t>;
using alVec4f = alVec4_t<float32_t>;
using alVec4i = alVec4_t<int32_t>;
using alVec4u = alVec4_t<uint32_t>;
using alVec3 = alVec3_t<real_t>;
using alVec4 = alVec4_t<real_t>;

#endif

