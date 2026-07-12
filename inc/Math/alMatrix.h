#ifndef _AL_MATRIXH_
#define _AL_MATRIXH_

template<typename Type>
class alMatrix_t
{
public:
	alVec4_t<Type> m_data[4u]; //< components

	alMatrix_t<Type>()
	{
		Identity();
	}

	alMatrix_t<Type>(const alMatrix_t<Type>& m)
	{
		*this = m;
	}

	alMatrix_t<Type>(const alQuaternion& q)
	{
		Identity();
		SetRotation(q);
	}

	alMatrix_t<Type>(Type v)
	{
		Fill(v);
	}

	alMatrix_t<Type>(
		const alVec4_t<Type>& x, 
		const alVec4_t<Type>& y, 
		const alVec4_t<Type>& z, 
		const alVec4_t<Type>& w)
	{
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
		m_data[3] = w;
	}

	void Fill(Type v) 
	{
		m_data[0].Set(v);
		m_data[1].Set(v);
		m_data[2].Set(v);
		m_data[3].Set(v);
	}

	void Zero() 
	{
		Fill(0.f);
	}


	void Identity() 
	{
		auto* p = this->GetPtr();
		p[0] = 1.f;
		p[1] = 0.f;
		p[2] = 0.f;
		p[3] = 0.f;

		p[4] = 0.f;
		p[5] = 1.f;
		p[6] = 0.f;
		p[7] = 0.f;

		p[8] = 0.f;
		p[9] = 0.f;
		p[10] = 1.f;
		p[11] = 0.f;

		p[12] = 0.f;
		p[13] = 0.f;
		p[14] = 0.f;
		p[15] = 1.f;

	}

	void SetValue(Type xx, Type xy, Type xz,
		Type yx, Type yy, Type yz,
		Type zx, Type zy, Type zz)
	{
		m_data[0].Set(xx, xy, xz, 0.f);
		m_data[1].Set(yx, yy, yz, 0.f);
		m_data[2].Set(zx, zy, zz, 0.f);
	}

	void SetRotation(const alQuaternion& q)
	{
		Type d = q.Length2();
		Type s = 2.0f / d;
		Type xs = q.x * s, ys = q.y * s, zs = q.z * s;
		Type wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		Type xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		Type yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
		SetValue(
			1.0f - (yy + zz), xy - wz, xz + wy,
			xy + wz, 1.0f - (xx + zz), yz - wx,
			xz - wy, yz + wx, 1.0f - (xx + yy));
	}

	void SetRotation(const alVec3f& dir, const alVec3f& up = alVec3f(0.f, 1.f, 0.f))
	{
		alVec3f xaxis = up.Cross(dir);
		xaxis.Normalize2();

		alVec3f yaxis = dir.Cross(xaxis);
		yaxis.Normalize2();

		m_data[0].x = xaxis.x;
		m_data[0].y = yaxis.x;
		m_data[0].z = dir.x;

		m_data[1].x = xaxis.y;
		m_data[1].y = yaxis.y;
		m_data[1].z = dir.y;

		m_data[2].x = xaxis.z;
		m_data[2].y = yaxis.z;
		m_data[2].z = dir.z;
	}

	Type* GetPtr() { return reinterpret_cast<Type*>(&m_data); }
	Type* GetPtrConst()const { return (Type*)&m_data; }

	alVec4_t<Type>& operator[](uint32_t i) { AL_ASSERT_ST(i <= 3); return m_data[i]; }
	const alVec4_t<Type>& operator[](uint32_t i) const { AL_ASSERT_ST(i <= 3); return m_data[i]; }

	//	add
	// \param m: other matrix
	// \return new matrix
	alMatrix_t<Type> operator+(const alMatrix_t<Type>& m) const
	{
		alMatrix_t<Type> out = *this;

		out[0] += m[0];
		out[1] += m[1];
		out[2] += m[2];
		out[3] += m[3];

		return out;
	}

	//	substract
	// \param m: other matrix
	// \return new matrix
	alMatrix_t<Type> operator-(const alMatrix_t<Type>& m) const
	{
		alMatrix_t<Type> out = *this;

		out[0] -= m[0];
		out[1] -= m[1];
		out[2] -= m[2];
		out[3] -= m[3];

		return out;
	}

	//	multiplication
	// \param m: other matrix
	// \return new matrix
	alMatrix_t<Type> operator*(const alMatrix_t<Type>& m) const
	{
		return alMatrix_t<Type>(
			m_data[0] * m[0].x + m_data[1] * m[0].y + m_data[2] * m[0].z + m_data[3] * m[0].w,
			m_data[0] * m[1].x + m_data[1] * m[1].y + m_data[2] * m[1].z + m_data[3] * m[1].w,
			m_data[0] * m[2].x + m_data[1] * m[2].y + m_data[2] * m[2].z + m_data[3] * m[2].w,
			m_data[0] * m[3].x + m_data[1] * m[3].y + m_data[2] * m[3].z + m_data[3] * m[3].w
		);
	}

	// возможно тут нужно по другому.
	alVec4f operator*(const alVec4f& v) const {
		return alVec4f
		(
			v.x * m_data[0].x + v.y * m_data[1].x + v.z * m_data[2].x + v.w * m_data[2].x,
			v.x * m_data[0].y + v.y * m_data[1].y + v.z * m_data[2].y + v.w * m_data[2].y,
			v.x * m_data[0].z + v.y * m_data[1].z + v.z * m_data[2].z + v.w * m_data[2].z,
			v.x * m_data[0].w + v.y * m_data[1].w + v.z * m_data[2].w + v.w * m_data[2].w
		);
	}

	//	divide
	// \param m: other matrix
	// \return new matrix
	alMatrix_t<Type> operator/(const alMatrix_t<Type>& m) const
	{
		alMatrix_t<Type> out = *this;

		out[0] /= m[0];
		out[1] /= m[1];
		out[2] /= m[2];
		out[3] /= m[3];

		return out;
	}

	//	add
	// \param m: other matrix
	// \return this matrix
	alMatrix_t<Type>& operator+=(const alMatrix_t<Type>& m) 
	{
		m_data[0] += m[0];
		m_data[1] += m[1];
		m_data[2] += m[2];
		m_data[3] += m[3];
		return *this;
	}

	//	substract
	// \param m: other matrix
	// \return this matrix
	alMatrix_t<Type>& operator-=(const alMatrix_t<Type>& m)
	{
		m_data[0] -= m[0];
		m_data[1] -= m[1];
		m_data[2] -= m[2];
		m_data[3] -= m[3];
		return *this;
	}

	//	multiplication
	// \param m: other matrix
	// \return this matrix
	alMatrix_t<Type>& operator*=(const alMatrix_t<Type>& m)
	{
		(*this) = (*this) * m;
		return *this;
	}

	//	divide
	// \param m: other matrix
	// \return this matrix
	alMatrix_t<Type>& operator/=(const alMatrix_t<Type>& m)
	{
		m_data[0] /= m[0];
		m_data[1] /= m[1];
		m_data[2] /= m[2];
		m_data[3] /= m[3];
		return *this;
	}

	//	transpose
	void Transpose()
	{
		alMatrix_t<Type> tmp;
		tmp[0].x = this->m_data[0].x; //0
		tmp[0].y = this->m_data[1].x; //1
		tmp[0].z = this->m_data[2].x; //2
		tmp[0].w = this->m_data[3].x; //3

		tmp[1].x = this->m_data[0].y; //4
		tmp[1].y = this->m_data[1].y; //5
		tmp[1].z = this->m_data[2].y; //6
		tmp[1].w = this->m_data[3].y; //7

		tmp[2].x = this->m_data[0].z; //8
		tmp[2].y = this->m_data[1].z; //9
		tmp[2].z = this->m_data[2].z; //10
		tmp[2].w = this->m_data[3].z; //11

		tmp[3].x = this->m_data[0].w; //12
		tmp[3].y = this->m_data[1].w; //13
		tmp[3].z = this->m_data[2].w; //14
		tmp[3].w = this->m_data[3].w; //15
		this->m_data[0] = tmp[0];
		this->m_data[1] = tmp[1];
		this->m_data[2] = tmp[2];
		this->m_data[3] = tmp[3];
	}

	//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
	bool Invert()
	{
		alMatrix_t<Type> mat;
		auto ptr = this->GetPtr();
		for (unsigned column = 0; column < 4; ++column)
		{
			// Swap row in case our pivot point is not working
			auto column_data = m_data[column].Data();
			if (column_data[column] == 0)
			{
				unsigned big = column;
				for (unsigned row = 0; row < 4; ++row)
				{
					auto row_data = m_data[row].Data();
					auto big_data = m_data[big].Data();
					if (fabs(row_data[column]) > fabs(big_data[column]))
						big = row;
				}
				// Print this is a singular matrix, return Identity ?
				if (big == column)
					fprintf(stderr, "Singular matrix\n");
				// Swap rows                               
				else for (unsigned j = 0; j < 4; ++j)
				{
					auto big_data = m_data[big].Data();
					std::swap(column_data[j], big_data[j]);

					auto other_column_data = mat.m_data[column].Data();
					auto other_big_data = mat.m_data[big].Data();
					std::swap(other_column_data[j], other_big_data[j]);
				}
			}

			// Set each row in the column to 0  
			for (unsigned row = 0; row < 4; ++row)
			{
				if (row != column)
				{
					auto row_data = m_data[row].Data();
					Type coeff = row_data[column] / column_data[column];
					if (coeff != 0)
					{
						for (unsigned j = 0; j < 4; ++j)
						{
							row_data[j] -= coeff * column_data[j];

							auto other_row_data = mat.m_data[row].Data();
							auto other_column_data = mat.m_data[column].Data();
							other_row_data[j] -= coeff * other_column_data[j];
						}
						// Set the element to 0 for safety
						row_data[column] = 0;
					}
				}
			}
		}

		// Set each element of the diagonal to 1
		for (unsigned row = 0; row < 4; ++row)
		{
			for (unsigned column = 0; column < 4; ++column)
			{
				auto other_row_data = mat.m_data[row].Data();
				auto row_data = m_data[row].Data();
				other_row_data[column] /= row_data[row];
			}
		}

		*this = mat;
		return true;
	}

	bool Invert2()
	{
		alMatrix_t<Type> r0, r1, r2, r3;
		Type a, det, invDet;
		Type* mat = reinterpret_cast<Type*>(this);

		det = mat[0 * 4 + 0] * mat[1 * 4 + 1] - mat[0 * 4 + 1] * mat[1 * 4 + 0];
		if (std::abs(det) < 1e-14) {
			return false;
		}
		invDet = 1.0f / det;

		r0[0][0] = mat[1 * 4 + 1] * invDet;
		r0[0][1] = -mat[0 * 4 + 1] * invDet;
		r0[1][0] = -mat[1 * 4 + 0] * invDet;
		r0[1][1] = mat[0 * 4 + 0] * invDet;
		r1[0][0] = r0[0][0] * mat[0 * 4 + 2] + r0[0][1] * mat[1 * 4 + 2];
		r1[0][1] = r0[0][0] * mat[0 * 4 + 3] + r0[0][1] * mat[1 * 4 + 3];
		r1[1][0] = r0[1][0] * mat[0 * 4 + 2] + r0[1][1] * mat[1 * 4 + 2];
		r1[1][1] = r0[1][0] * mat[0 * 4 + 3] + r0[1][1] * mat[1 * 4 + 3];
		r2[0][0] = mat[2 * 4 + 0] * r1[0][0] + mat[2 * 4 + 1] * r1[1][0];
		r2[0][1] = mat[2 * 4 + 0] * r1[0][1] + mat[2 * 4 + 1] * r1[1][1];
		r2[1][0] = mat[3 * 4 + 0] * r1[0][0] + mat[3 * 4 + 1] * r1[1][0];
		r2[1][1] = mat[3 * 4 + 0] * r1[0][1] + mat[3 * 4 + 1] * r1[1][1];
		r3[0][0] = r2[0][0] - mat[2 * 4 + 2];
		r3[0][1] = r2[0][1] - mat[2 * 4 + 3];
		r3[1][0] = r2[1][0] - mat[3 * 4 + 2];
		r3[1][1] = r2[1][1] - mat[3 * 4 + 3];

		det = r3[0][0] * r3[1][1] - r3[0][1] * r3[1][0];
		if (std::abs(det) < 1e-14) {
			return false;
		}

		invDet = 1.0f / det;

		a = r3[0][0];
		r3[0][0] = r3[1][1] * invDet;
		r3[0][1] = -r3[0][1] * invDet;
		r3[1][0] = -r3[1][0] * invDet;
		r3[1][1] = a * invDet;
		r2[0][0] = mat[2 * 4 + 0] * r0[0][0] + mat[2 * 4 + 1] * r0[1][0];
		r2[0][1] = mat[2 * 4 + 0] * r0[0][1] + mat[2 * 4 + 1] * r0[1][1];
		r2[1][0] = mat[3 * 4 + 0] * r0[0][0] + mat[3 * 4 + 1] * r0[1][0];
		r2[1][1] = mat[3 * 4 + 0] * r0[0][1] + mat[3 * 4 + 1] * r0[1][1];
		mat[2 * 4 + 0] = r3[0][0] * r2[0][0] + r3[0][1] * r2[1][0];
		mat[2 * 4 + 1] = r3[0][0] * r2[0][1] + r3[0][1] * r2[1][1];
		mat[3 * 4 + 0] = r3[1][0] * r2[0][0] + r3[1][1] * r2[1][0];
		mat[3 * 4 + 1] = r3[1][0] * r2[0][1] + r3[1][1] * r2[1][1];
		mat[0 * 4 + 0] = r0[0][0] - r1[0][0] * mat[2 * 4 + 0] - r1[0][1] * mat[3 * 4 + 0];
		mat[0 * 4 + 1] = r0[0][1] - r1[0][0] * mat[2 * 4 + 1] - r1[0][1] * mat[3 * 4 + 1];
		mat[1 * 4 + 0] = r0[1][0] - r1[1][0] * mat[2 * 4 + 0] - r1[1][1] * mat[3 * 4 + 0];
		mat[1 * 4 + 1] = r0[1][1] - r1[1][0] * mat[2 * 4 + 1] - r1[1][1] * mat[3 * 4 + 1];
		mat[0 * 4 + 2] = r1[0][0] * r3[0][0] + r1[0][1] * r3[1][0];
		mat[0 * 4 + 3] = r1[0][0] * r3[0][1] + r1[0][1] * r3[1][1];
		mat[1 * 4 + 2] = r1[1][0] * r3[0][0] + r1[1][1] * r3[1][0];
		mat[1 * 4 + 3] = r1[1][0] * r3[0][1] + r1[1][1] * r3[1][1];
		mat[2 * 4 + 2] = -r3[0][0];
		mat[2 * 4 + 3] = -r3[0][1];
		mat[3 * 4 + 2] = -r3[1][0];
		mat[3 * 4 + 3] = -r3[1][1];

		return true;
	}

	void SetTranslation(const alVec3f& v) { m_data[3].Set(v.x, v.y, v.z, 1.f); }
	void SetTranslation(const alVec4f& v) { m_data[3].Set(v.x, v.y, v.z, 1.f); }
	void SetScale(const alVec3f& v)
	{
		m_data[0].x = v.x;
		m_data[1].y = v.y;
		m_data[2].z = v.z;
	}

	void SetScale(const alVec4f& v)
	{
		m_data[0].x = v.x;
		m_data[1].y = v.y;
		m_data[2].z = v.z;
	}

	void SetBasis(const alMatrix_t<Type>& other)
	{
		m_data[0].x = other.m_data[0].x;
		m_data[0].y = other.m_data[0].y;
		m_data[0].z = other.m_data[0].z;
		m_data[1].x = other.m_data[1].x;
		m_data[1].y = other.m_data[1].y;
		m_data[1].z = other.m_data[1].z;
		m_data[2].x = other.m_data[2].x;
		m_data[2].y = other.m_data[2].y;
		m_data[2].z = other.m_data[2].z;
	}

	alMatrix_t<Type> GetBasis() 
	{
		alMatrix_t<Type> other;
		other.m_data[0].x = m_data[0].x;
		other.m_data[0].y = m_data[0].y;
		other.m_data[0].z = m_data[0].z;
		other.m_data[1].x = m_data[1].x;
		other.m_data[1].y = m_data[1].y;
		other.m_data[1].z = m_data[1].z;
		other.m_data[2].x = m_data[2].x;
		other.m_data[2].y = m_data[2].y;
		other.m_data[2].z = m_data[2].z;
		return other;
	}
};

class alMat3
{
public:
	alVec3f m_data[3];

	alMat3()
	{
		Identity();
	}

	alMat3(const alMat3& m)
	{
		*this = m;
	}

	alMat3(const alQuaternion& q)
	{
		SetRotation(q);
	}

	alMat3(float32_t v)
	{
		Fill(v);
	}

	alMat3(const alVec3f& x, const alVec3f& y, const alVec3f& z) 
	{
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
	}

	void Fill(float32_t v) 
	{
		m_data[0].Set(v);
		m_data[1].Set(v);
		m_data[2].Set(v);
	}

	void Zero() 
	{
		Fill(0.f);
	}


	void Identity() 
	{
		auto* p = this->GetPtr();
		p[0] = 1.f;
		p[1] = 0.f;
		p[2] = 0.f;

		p[3] = 0.f;
		p[4] = 1.f;
		p[5] = 0.f;

		p[6] = 0.f;
		p[7] = 0.f;
		p[8] = 1.f;
	}

	void SetValue(float32_t xx, float32_t xy, float32_t xz,
		float32_t yx, float32_t yy, float32_t yz,
		float32_t zx, float32_t zy, float32_t zz)
	{
		m_data[0].Set(xx, xy, xz);
		m_data[1].Set(yx, yy, yz);
		m_data[2].Set(zx, zy, zz);
	}

	void SetScale(const alVec3f& v)
	{
		m_data[0].x = v.x;
		m_data[1].y = v.y;
		m_data[2].z = v.z;
	}

	void SetScale(const alVec4& v)
	{
		m_data[0].x = (float32_t)v.x;
		m_data[1].y = (float32_t)v.y;
		m_data[2].z = (float32_t)v.z;
	}

	void SetRotation(const alQuaternion& q)
	{
		float32_t d = q.Length2();
		float32_t s = 2.0f / d;
		float32_t xs = q.x * s, ys = q.y * s, zs = q.z * s;
		float32_t wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		float32_t xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		float32_t yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
		SetValue(
			1.0f - (yy + zz), xy - wz, xz + wy,
			xy + wz, 1.0f - (xx + zz), yz - wx,
			xz - wy, yz + wx, 1.0f - (xx + yy));
	}

	float32_t* GetPtr() { return reinterpret_cast<float32_t*>(&m_data); }
	float32_t* GetPtrConst()const { return (float32_t*)&m_data; }

	alVec3f& operator[](uint32_t i) { AL_ASSERT_ST(i <= 3); return m_data[i]; }
	const alVec3f& operator[](uint32_t i) const { AL_ASSERT_ST(i <= 3); return m_data[i]; }
};

using alMat4 = alMatrix_t<real_t>;

#endif

