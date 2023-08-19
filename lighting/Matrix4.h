#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cassert>
#include <cmath>
#include "Vector3.h"

namespace __hidden__
{
	/// the PI constant
	const long double PI = atan2l(0., -1.);
}

/** A 4x4 matrix of scalar values.
 *  This matrix is *COLUMN MAJOR*, which means the 16 elements of the matrix
 *  should be interpreted like this:
 *   M[0]  M[4]  M[8]  M[12]
 *   M[1]  M[5]  M[9]  M[13]
 *   M[2]  M[6]  M[10] M[14]
 *   M[3]  M[7]  M[11] M[15]
 */
template <class T>
class Matrix4
{

	typedef T data_t;

	// ********************************************************************************************************
	// *** Static methods *************************************************************************************
public:
	/// Return a matrix representing a translation according to the specified vector
	template <class U>
	static Matrix4<data_t> createTranslation(const Vector3<U> &tr)
	{
		Matrix4<data_t> m;
		m.setTranslation(tr);
		return m;
	}

	/// Return a matrix representing a scaling according to the specified values
	template <class U>
	static Matrix4<data_t> createScaling(const U &sx, const U &sy, const U &sz)
	{
		Matrix4<data_t> m;
		m.setScaling(sx, sy, sz);
		return m;
	}
	template <class U>
	static Matrix4<data_t> createScaling(const Vector3<U> &s)
	{
		Matrix4<data_t> m;
		m.setScaling(s.x(), s.y(), s.z());
		return m;
	}

	/// Return a matrix representing a rotation of 'angle' degrees around the specified axis.
	template <class U, class V>
	static Matrix4<data_t> createRotation(const U &angle, const Vector3<V> &rotationAxis)
	{
		const long double x = static_cast<long double>(rotationAxis.x());
		const long double y = static_cast<long double>(rotationAxis.y());
		const long double z = static_cast<long double>(rotationAxis.z());
		long double c = cosl((angle * 2. * __hidden__::PI) / 360.);
		long double s = sinl((angle * 2. * __hidden__::PI) / 360.);

		return Matrix4<data_t>(
			static_cast<data_t>(x * x * (1 - c) + c),
			static_cast<data_t>(x * y * (1 - c) - z * s),
			static_cast<data_t>(x * z * (1 - c) + y * s),
			data_t(0),
			static_cast<data_t>(y * x * (1 - c) + z * s),
			static_cast<data_t>(y * y * (1 - c) + c),
			static_cast<data_t>(y * z * (1 - c) - x * s),
			data_t(0),
			static_cast<data_t>(z * x * (1 - c) - y * s),
			static_cast<data_t>(z * y * (1 - c) + x * s),
			static_cast<data_t>(z * z * (1 - c) + c),
			data_t(0),
			data_t(0), data_t(0), data_t(0), data_t(1));
	}

	/// Return an orthographic projection matrix according to the specified parameters
	template <class U>
	static Matrix4<data_t> createOrthoPrj(
		const U &left, const U &right, const U &bottom, const U &top,
		const U &znear, const U &zfar)
	{
		long double width = right - left;
		assert(width != 0.l);
		long double height = top - bottom;
		assert(height != 0.l);
		long double depth = zfar - znear;
		assert(depth != 0.l);
		data_t sx = static_cast<data_t>(2. / width);
		data_t sy = static_cast<data_t>(2. / height);
		data_t sz = static_cast<data_t>(-2. / depth);
		data_t tx = static_cast<data_t>(-(left + right) / width);
		data_t ty = static_cast<data_t>(-(top + bottom) / height);
		data_t tz = static_cast<data_t>(-(zfar + znear) / depth);

		return Matrix4<data_t>(sx, data_t(0), data_t(0), tx,
							   data_t(0), sy, data_t(0), ty,
							   data_t(0), data_t(0), sz, tz,
							   data_t(0), data_t(0), data_t(0), data_t(1));
	}

	/// Return a perspective projection matrix according to the specified parameters
	template <class U>
	static Matrix4<data_t> createPerspectivePrj(
		const U &fov, const U &aspectRatio, const U &znear, const U &zfar)
	{
		long double top = znear * tanl(fov * __hidden__::PI / 360.);
		long double bottom = -top;
		long double left = bottom * aspectRatio;
		long double right = top * aspectRatio;

		long double k = 2.0 * znear;
		long double width = right - left;
		assert(width != 0.l);
		long double height = top - bottom;
		assert(height != 0.l);
		long double depth = zfar - znear;
		assert(depth != 0.l);

		data_t xx = static_cast<data_t>(k / width);
		data_t yy = static_cast<data_t>(k / height);
		data_t xz = static_cast<data_t>((right + left) / width);
		data_t yz = static_cast<data_t>((top + bottom) / height);
		data_t zz = static_cast<data_t>((zfar + znear) / -depth);
		data_t zw = static_cast<data_t>((-k * zfar) / depth);

		return Matrix4<data_t>(xx, data_t(0), xz, data_t(0),
							   data_t(0), yy, yz, data_t(0),
							   data_t(0), data_t(0), zz, zw,
							   data_t(0), data_t(0), data_t(-1), data_t(0));
	}

	// ********************************************************************************************************
	// *** Basic methods **************************************************************************************
public:
	/// Default constructor. Create an identity matrix
	Matrix4()
	{
		this->identity();
	}

	/// Create a matrix using the specified values. Values must be specified column-wise.
	template <class U>
	Matrix4(const U &val0, const U &val4, const U &val8, const U &val12,
			const U &val1, const U &val5, const U &val9, const U &val13,
			const U &val2, const U &val6, const U &val10, const U &val14,
			const U &val3, const U &val7, const U &val11, const U &val15)
	{
		mElements[0] = val0;
		mElements[4] = val4;
		mElements[8] = val8;
		mElements[12] = val12;
		mElements[1] = val1;
		mElements[5] = val5;
		mElements[9] = val9;
		mElements[13] = val13;
		mElements[2] = val2;
		mElements[6] = val6;
		mElements[10] = val10;
		mElements[14] = val14;
		mElements[3] = val3;
		mElements[7] = val7;
		mElements[11] = val11;
		mElements[15] = val15;
	}

	/// Create a matrix using the specified vectors. Each vector represent a column
	template <class U>
	Matrix4(const Vector3<U> &col0, const Vector3<U> &col1, const Vector3<U> &col2,
			const Vector3<U> &col3 = Vector3<U>(0., 0., 0.))
	{
		mElements[0] = static_cast<data_t>(col0.x());
		mElements[1] = static_cast<data_t>(col0.y());
		mElements[2] = static_cast<data_t>(col0.z());
		mElements[3] = data_t(0);
		mElements[4] = static_cast<data_t>(col1.x());
		mElements[5] = static_cast<data_t>(col1.y());
		mElements[6] = static_cast<data_t>(col1.z());
		mElements[7] = data_t(0);
		mElements[8] = static_cast<data_t>(col2.x());
		mElements[9] = static_cast<data_t>(col2.y());
		mElements[10] = static_cast<data_t>(col2.z());
		mElements[11] = data_t(0);
		mElements[12] = static_cast<data_t>(col3.x());
		mElements[13] = static_cast<data_t>(col3.y());
		mElements[14] = static_cast<data_t>(col3.z());
		mElements[15] = data_t(1);
	}

	/// Create a matrix using the specified values. Values must be specified column-wise.
	template <class U>
	Matrix4(const U pVals[16])
	{
		this->set(pVals);
	}

	/// Create a matrix using the specified values.
	template <class U>
	Matrix4(const U pVals[4][4])
	{
		mElements[0] = static_cast<data_t>(pVals[0][0]);
		mElements[1] = static_cast<data_t>(pVals[0][1]);
		mElements[2] = static_cast<data_t>(pVals[0][2]);
		mElements[3] = static_cast<data_t>(pVals[0][3]);
		mElements[4] = static_cast<data_t>(pVals[1][0]);
		mElements[5] = static_cast<data_t>(pVals[1][1]);
		mElements[6] = static_cast<data_t>(pVals[1][2]);
		mElements[7] = static_cast<data_t>(pVals[1][3]);
		mElements[8] = static_cast<data_t>(pVals[2][0]);
		mElements[9] = static_cast<data_t>(pVals[2][1]);
		mElements[10] = static_cast<data_t>(pVals[2][2]);
		mElements[11] = static_cast<data_t>(pVals[2][3]);
		mElements[12] = static_cast<data_t>(pVals[3][0]);
		mElements[13] = static_cast<data_t>(pVals[3][1]);
		mElements[14] = static_cast<data_t>(pVals[3][2]);
		mElements[15] = static_cast<data_t>(pVals[3][3]);
	}

	/// Copy constructor
	template <class U>
	Matrix4(const Matrix4<U> &other)
	{
		this->set(other.mElements);
	}

	/// Assignment operator
	template <class U>
	Matrix4<data_t> &operator=(const Matrix4<U> &other)
	{
		if (&other != this)
			this->set(other.mElements);
		return *this;
	}

	/// Destructor
	~Matrix4()
	{
	}

	// ********************************************************************************************
	// *** Getters and Setters ********************************************************************
public:
	/// Return a pointer to the matrix elements.
	data_t *get()
	{
		return mElements;
	}
	const data_t *get() const
	{
		return mElements;
	}

	/// Set the elements of the matrix.
	template <class U>
	void set(const U pVals[16])
	{
		mElements[0] = static_cast<data_t>(pVals[0]);
		mElements[1] = static_cast<data_t>(pVals[1]);
		mElements[2] = static_cast<data_t>(pVals[2]);
		mElements[3] = static_cast<data_t>(pVals[3]);
		mElements[4] = static_cast<data_t>(pVals[4]);
		mElements[5] = static_cast<data_t>(pVals[5]);
		mElements[6] = static_cast<data_t>(pVals[6]);
		mElements[7] = static_cast<data_t>(pVals[7]);
		mElements[8] = static_cast<data_t>(pVals[8]);
		mElements[9] = static_cast<data_t>(pVals[9]);
		mElements[10] = static_cast<data_t>(pVals[10]);
		mElements[11] = static_cast<data_t>(pVals[11]);
		mElements[12] = static_cast<data_t>(pVals[12]);
		mElements[13] = static_cast<data_t>(pVals[13]);
		mElements[14] = static_cast<data_t>(pVals[14]);
		mElements[15] = static_cast<data_t>(pVals[15]);
	}

	/// Return the matrix element at the specified index.
	data_t &get(unsigned int i)
	{
		assert(i < 16);
		return mElements[i];
	}
	const data_t &get(unsigned int i) const
	{
		assert(i < 16);
		return mElements[i];
	}

	/// Access the matrix element at the specified index.
	data_t &operator()(unsigned int i)
	{
		return get(i);
	}
	const data_t &operator()(unsigned int i) const
	{
		return get(i);
	}

	/// Access the matrix element at the specified index.
	data_t &operator[](unsigned int i)
	{
		return get(i);
	}
	const data_t &operator[](unsigned int i) const
	{
		return get(i);
	}

	/// Set the value of the matrix element at the specified index.
	template <class U>
	void set(unsigned int i, const U &val)
	{
		get(i) = val;
	}

	/// Return the matrix element at (row, col).
	data_t &get(unsigned int row, unsigned int col)
	{
		return get(4 * col + row);
	}
	const data_t &get(unsigned int row, unsigned int col) const
	{
		return get(4 * col + row);
	}

	/// Access the matrix element at (row, col).
	data_t &operator()(unsigned int row, unsigned int col)
	{
		return get(row, col);
	}
	const data_t &operator()(unsigned int row, unsigned int col) const
	{
		return get(row, col);
	}

	/// Set the value of the matrix element at (row, col).
	template <class U>
	void set(unsigned int row, unsigned int col, const U &val)
	{
		get(row, col) = val;
	}

	// ********************************************************************************************
	// *** Matrix manipulation ********************************************************************
public:
	/// Set the identity matrix as the current matrix.
	void identity()
	{
		mElements[0] = mElements[5] = mElements[10] = mElements[15] = data_t(1);
		mElements[1] = mElements[2] = mElements[3] = mElements[4] = mElements[6] =
			mElements[7] = mElements[8] = mElements[9] = mElements[11] = mElements[12] =
				mElements[13] = mElements[14] = data_t(0);
	}

	/// Set/get the translation part of the matrix.
	template <class U>
	void setTranslation(const U &tx, const U &ty, const U &tz)
	{
		mElements[12] = tx;
		mElements[13] = ty;
		mElements[14] = tz;
	}
	template <class U>
	void setTranslation(const Vector3<U> &t)
	{
		setTranslation(t.x(), t.y(), t.z());
	}
	Vector3<data_t> getTranslation() const
	{
		return Vector3<data_t>(mElements[12], mElements[13], mElements[14]);
	}

	/// Return a matrix obtained post-multiplying this matrix by a translation matrix.
	template <class U>
	Matrix4<data_t> getTranslated(const U &tx, const U &ty, const U &tz) const
	{
		return (*this) * createTranslation(tx, ty, tz);
	}
	template <class U>
	Matrix4<data_t> getTranslated(const Vector3<U> &t) const
	{
		return getTranslated(t.x(), t.y(), t.z());
	}

	/// Post-multiply this matrix by a translation matrix.
	template <class U>
	void translate(const Vector3<U> &vecTranslation)
	{
		*this = getTranslated(vecTranslation);
	}

	/// Set/get the scaling components of the matrix.
	template <class U>
	void setScaling(const U &sx, const U &sy, const U &sz)
	{
		mElements[0] = sx;
		mElements[5] = sy;
		mElements[10] = sz;
	}
	template <class U>
	void setScaling(const Vector3<U> &s)
	{
		setScaling(s.x(), s.y(), s.z());
	}
	Vector3<data_t> getScaling() const
	{
		return Vector3<data_t>(mElements[0], mElements[5], mElements[10]);
	}

	/// Return a matrix obtained post-multiplying this matrix by a scaling matrix.
	template <class U>
	Matrix4<data_t> getScaled(const U &sx, const U &sy, const U &sz) const
	{
		return (*this) * createScaling(sx, sy, sz);
	}
	template <class U>
	Matrix4<data_t> getScaled(const Vector3<U> &s) const
	{
		return getScaled(s.x(), s.y(), s.z());
	}

	/// Post-multiply this matrix by a scaling matrix.
	template <class U>
	void scale(const Vector3<U> &s)
	{
		*this = getScaled(s);
	}

	/// Return a matrix obtained post-multiplying this matrix by a rotation matrix.
	template <class U, class V>
	Matrix4<data_t> getRotated(const U &angle, const Vector3<V> &rotationAxis) const
	{
		return (*this) * createRotation(angle, rotationAxis);
	}

	/// Post-multiply this matrix by a rotation matrix.
	template <class U, class V>
	void rotate(const U &angle, const Vector3<V> &rotationAxis)
	{
		*this = getRotated(angle, rotationAxis);
	}

	/// Return the inverse of this matrix
	Matrix4<data_t> getInverse() const
	{
		const long double TMP_1 = mElements[10] * mElements[15];
		const long double TMP_2 = mElements[4] * TMP_1;
		const long double TMP_4 = mElements[14] * mElements[11];
		const long double TMP_5 = mElements[4] * TMP_4;
		const long double TMP_6 = mElements[8] * mElements[15];
		const long double TMP_7 = mElements[6] * TMP_6;
		const long double TMP_8 = mElements[12] * mElements[11];
		const long double TMP_9 = mElements[6] * TMP_8;
		const long double TMP_11 = mElements[8] * mElements[14];
		const long double TMP_12 = mElements[7] * TMP_11;
		const long double TMP_14 = mElements[12] * mElements[10];
		const long double TMP_15 = mElements[7] * TMP_14;
		const long double TMP_17 = mElements[5] * TMP_1;
		const long double TMP_19 = mElements[5] * TMP_4;
		const long double TMP_22 = mElements[9] * mElements[15];
		const long double TMP_23 = mElements[6] * TMP_22;
		const long double TMP_26 = mElements[13] * mElements[11];
		const long double TMP_27 = mElements[6] * TMP_26;
		const long double TMP_29 = mElements[9] * mElements[14];
		const long double TMP_30 = mElements[7] * TMP_29;
		const long double TMP_32 = mElements[13] * mElements[10];
		const long double TMP_33 = mElements[7] * TMP_32;
		const long double TMP_48 = mElements[4] * TMP_22;
		const long double TMP_50 = mElements[4] * TMP_26;
		const long double TMP_53 = mElements[5] * TMP_6;
		const long double TMP_56 = mElements[5] * TMP_8;
		const long double TMP_58 = mElements[8] * mElements[13];
		const long double TMP_59 = mElements[7] * TMP_58;
		const long double TMP_61 = mElements[12] * mElements[9];
		const long double TMP_62 = mElements[7] * TMP_61;
		const long double TMP_66 = mElements[4] * TMP_29;
		const long double TMP_69 = mElements[4] * TMP_32;
		const long double TMP_71 = mElements[5] * TMP_11;
		const long double TMP_73 = mElements[5] * TMP_14;
		const long double TMP_76 = mElements[6] * TMP_58;
		const long double TMP_79 = mElements[6] * TMP_61;
		const long double TMP_84 = 1.0 /
								   (mElements[0] * TMP_17 - mElements[0] * TMP_19 - mElements[0] * TMP_23 +
									mElements[0] * TMP_27 + mElements[0] * TMP_30 - mElements[0] * TMP_33 -
									mElements[1] * TMP_2 + mElements[1] * TMP_5 + mElements[1] * TMP_7 -
									mElements[1] * TMP_9 - mElements[1] * TMP_12 + mElements[1] * TMP_15 +
									mElements[2] * TMP_48 - mElements[2] * TMP_50 - mElements[2] * TMP_53 +
									mElements[2] * TMP_56 + mElements[2] * TMP_59 - mElements[2] * TMP_62 -
									mElements[3] * TMP_66 + mElements[3] * TMP_69 + mElements[3] * TMP_71 -
									mElements[3] * TMP_73 - mElements[3] * TMP_76 + mElements[3] * TMP_79);
		const long double TMP_116 = mElements[6] * mElements[11];
		const long double TMP_118 = mElements[10] * mElements[7];
		const long double TMP_121 = mElements[4] * mElements[11];
		const long double TMP_124 = mElements[8] * mElements[7];
		const long double TMP_126 = mElements[4] * mElements[10];
		const long double TMP_128 = mElements[8] * mElements[6];
		const long double TMP_133 = mElements[6] * mElements[15];
		const long double TMP_135 = mElements[14] * mElements[7];
		const long double TMP_138 = mElements[4] * mElements[15];
		const long double TMP_141 = mElements[12] * mElements[7];
		const long double TMP_143 = mElements[4] * mElements[14];
		const long double TMP_145 = mElements[12] * mElements[6];
		const long double TMP_151 = mElements[5] * mElements[10];
		const long double TMP_153 = mElements[9] * mElements[6];
		const long double TMP_159 = mElements[4] * mElements[9];
		const long double TMP_161 = mElements[8] * mElements[5];
		const long double TMP_166 = mElements[5] * mElements[15];
		const long double TMP_168 = mElements[13] * mElements[7];
		const long double TMP_174 = mElements[4] * mElements[13];
		const long double TMP_176 = mElements[12] * mElements[5];
		const long double TMP_187 = mElements[5] * mElements[14];
		const long double TMP_189 = mElements[13] * mElements[6];
		const long double TMP_212 = mElements[5] * mElements[11];
		const long double TMP_214 = mElements[9] * mElements[7];

		Matrix4<data_t> matNew;
		matNew.mElements[4] = (-TMP_2 + TMP_5 + TMP_7 - TMP_9 - TMP_12 + TMP_15) * TMP_84;
		matNew.mElements[8] = -(-TMP_48 + TMP_50 + TMP_53 - TMP_56 - TMP_59 + TMP_62) * TMP_84;
		matNew.mElements[9] = -(mElements[0] * TMP_22 - mElements[0] * TMP_26 - mElements[1] * TMP_6 + mElements[1] * TMP_8 + mElements[3] * TMP_58 - mElements[3] * TMP_61) * TMP_84;
		matNew.mElements[13] = -(-mElements[0] * TMP_29 + mElements[0] * TMP_32 + mElements[1] * TMP_11 - mElements[1] * TMP_14 - mElements[2] * TMP_58 + mElements[2] * TMP_61) * TMP_84;
		matNew.mElements[7] = (mElements[0] * TMP_116 - mElements[0] * TMP_118 - mElements[2] * TMP_121 + mElements[2] * TMP_124 + mElements[3] * TMP_126 - mElements[3] * TMP_128) * TMP_84;
		matNew.mElements[6] = -(mElements[0] * TMP_133 - mElements[0] * TMP_135 - mElements[2] * TMP_138 + mElements[2] * TMP_141 + mElements[3] * TMP_143 - mElements[3] * TMP_145) * TMP_84;
		matNew.mElements[15] = (mElements[0] * TMP_151 - mElements[0] * TMP_153 - mElements[1] * TMP_126 + mElements[1] * TMP_128 + mElements[2] * TMP_159 - mElements[2] * TMP_161) * TMP_84;
		matNew.mElements[10] = (mElements[0] * TMP_166 - mElements[0] * TMP_168 - mElements[1] * TMP_138 + mElements[1] * TMP_141 + mElements[3] * TMP_174 - mElements[3] * TMP_176) * TMP_84;
		matNew.mElements[2] = (mElements[1] * TMP_133 - mElements[1] * TMP_135 - mElements[2] * TMP_166 + mElements[2] * TMP_168 + mElements[3] * TMP_187 - mElements[3] * TMP_189) * TMP_84;
		matNew.mElements[1] = -(mElements[1] * TMP_1 - mElements[1] * TMP_4 - mElements[2] * TMP_22 + mElements[2] * TMP_26 + mElements[3] * TMP_29 - mElements[3] * TMP_32) * TMP_84;
		matNew.mElements[12] = -(TMP_66 - TMP_69 - TMP_71 + TMP_73 + TMP_76 - TMP_79) * TMP_84;
		matNew.mElements[11] = -(mElements[0] * TMP_212 - mElements[0] * TMP_214 - mElements[1] * TMP_121 + mElements[1] * TMP_124 + mElements[3] * TMP_159 - mElements[3] * TMP_161) * TMP_84;
		matNew.mElements[0] = (TMP_17 - TMP_19 - TMP_23 + TMP_27 + TMP_30 - TMP_33) * TMP_84;
		matNew.mElements[14] = -(mElements[0] * TMP_187 - mElements[0] * TMP_189 - mElements[1] * TMP_143 + mElements[1] * TMP_145 + mElements[2] * TMP_174 - mElements[2] * TMP_176) * TMP_84;
		matNew.mElements[3] = -(mElements[1] * TMP_116 - mElements[1] * TMP_118 - mElements[2] * TMP_212 + mElements[2] * TMP_214 + mElements[3] * TMP_151 - mElements[3] * TMP_153) * TMP_84;
		matNew.mElements[5] = (mElements[0] * TMP_1 - mElements[0] * TMP_4 - mElements[2] * TMP_6 + mElements[2] * TMP_8 + mElements[3] * TMP_11 - mElements[3] * TMP_14) * TMP_84;

		return matNew;
	}

	/// Invert this matrix
	void invert()
	{
		(*this) = getInverse();
	}

	/// Return the transposed of this matrix
	Matrix4<data_t> getTransposed() const
	{
		return Matrix4<data_t>(
			mElements[0], mElements[1], mElements[2], mElements[3],
			mElements[4], mElements[5], mElements[6], mElements[7],
			mElements[8], mElements[9], mElements[10], mElements[11],
			mElements[12], mElements[13], mElements[14], mElements[15]);
	}

	/// Transpose this matrix
	void transpose()
	{
		(*this) = getTransposed();
	}

	// ********************************************************************************************
	// *** Matrix operations **********************************************************************
public:
	/** Return the affine transformation (discard translation) of the specified
	 *  vector according to this matrix. */
	template <class U>
	const Vector3<U> affineMul(const Vector3<U> &v) const
	{
		return Vector3<U>(
			(static_cast<U>(mElements[0]) * v.x() + static_cast<U>(mElements[4]) * v.y() + static_cast<U>(mElements[8]) * v.z()),
			(static_cast<U>(mElements[1]) * v.x() + static_cast<U>(mElements[5]) * v.y() + static_cast<U>(mElements[9]) * v.z()),
			(static_cast<U>(mElements[2]) * v.x() + static_cast<U>(mElements[6]) * v.y() + static_cast<U>(mElements[10]) * v.z()));
	}

	/// Post-multiply this matrix by the specified one (this * m)
	template <class U>
	void mul(const Matrix4<U> &m)
	{
		postmul(m);
	}

	/// Post-multiply this matrix by the specified one (this * m)
	template <class U>
	void postmul(const Matrix4<U> &m)
	{
		*this = *this * m;
	}

	/// Pre-multiply this matrix by the specified one (m * this)
	template <class U>
	void premul(const Matrix4<U> &m)
	{
		*this = m * *this;
	}

	/// Return the product between this and the specified matrix (this * m)
	template <class U>
	Matrix4<data_t> operator*(const Matrix4<U> &m) const
	{
		return Matrix4<data_t>(
			mElements[0] * static_cast<data_t>(m.mElements[0]) + mElements[4] * static_cast<data_t>(m.mElements[1]) + mElements[8] * static_cast<data_t>(m.mElements[2]) + mElements[12] * static_cast<data_t>(m.mElements[3]),
			mElements[0] * static_cast<data_t>(m.mElements[4]) + mElements[4] * static_cast<data_t>(m.mElements[5]) + mElements[8] * static_cast<data_t>(m.mElements[6]) + mElements[12] * static_cast<data_t>(m.mElements[7]),
			mElements[0] * static_cast<data_t>(m.mElements[8]) + mElements[4] * static_cast<data_t>(m.mElements[9]) + mElements[8] * static_cast<data_t>(m.mElements[10]) + mElements[12] * static_cast<data_t>(m.mElements[11]),
			mElements[0] * static_cast<data_t>(m.mElements[12]) + mElements[4] * static_cast<data_t>(m.mElements[13]) + mElements[8] * static_cast<data_t>(m.mElements[14]) + mElements[12] * static_cast<data_t>(m.mElements[15]),

			mElements[1] * static_cast<data_t>(m.mElements[0]) + mElements[5] * static_cast<data_t>(m.mElements[1]) + mElements[9] * static_cast<data_t>(m.mElements[2]) + mElements[13] * static_cast<data_t>(m.mElements[3]),
			mElements[1] * static_cast<data_t>(m.mElements[4]) + mElements[5] * static_cast<data_t>(m.mElements[5]) + mElements[9] * static_cast<data_t>(m.mElements[6]) + mElements[13] * static_cast<data_t>(m.mElements[7]),
			mElements[1] * static_cast<data_t>(m.mElements[8]) + mElements[5] * static_cast<data_t>(m.mElements[9]) + mElements[9] * static_cast<data_t>(m.mElements[10]) + mElements[13] * static_cast<data_t>(m.mElements[11]),
			mElements[1] * static_cast<data_t>(m.mElements[12]) + mElements[5] * static_cast<data_t>(m.mElements[13]) + mElements[9] * static_cast<data_t>(m.mElements[14]) + mElements[13] * static_cast<data_t>(m.mElements[15]),

			mElements[2] * static_cast<data_t>(m.mElements[0]) + mElements[6] * static_cast<data_t>(m.mElements[1]) + mElements[10] * static_cast<data_t>(m.mElements[2]) + mElements[14] * static_cast<data_t>(m.mElements[3]),
			mElements[2] * static_cast<data_t>(m.mElements[4]) + mElements[6] * static_cast<data_t>(m.mElements[5]) + mElements[10] * static_cast<data_t>(m.mElements[6]) + mElements[14] * static_cast<data_t>(m.mElements[7]),
			mElements[2] * static_cast<data_t>(m.mElements[8]) + mElements[6] * static_cast<data_t>(m.mElements[9]) + mElements[10] * static_cast<data_t>(m.mElements[10]) + mElements[14] * static_cast<data_t>(m.mElements[11]),
			mElements[2] * static_cast<data_t>(m.mElements[12]) + mElements[6] * static_cast<data_t>(m.mElements[13]) + mElements[10] * static_cast<data_t>(m.mElements[14]) + mElements[14] * static_cast<data_t>(m.mElements[15]),

			mElements[3] * static_cast<data_t>(m.mElements[0]) + mElements[7] * static_cast<data_t>(m.mElements[1]) + mElements[11] * static_cast<data_t>(m.mElements[2]) + mElements[15] * static_cast<data_t>(m.mElements[3]),
			mElements[3] * static_cast<data_t>(m.mElements[4]) + mElements[7] * static_cast<data_t>(m.mElements[5]) + mElements[11] * static_cast<data_t>(m.mElements[6]) + mElements[15] * static_cast<data_t>(m.mElements[7]),
			mElements[3] * static_cast<data_t>(m.mElements[8]) + mElements[7] * static_cast<data_t>(m.mElements[9]) + mElements[11] * static_cast<data_t>(m.mElements[10]) + mElements[15] * static_cast<data_t>(m.mElements[11]),
			mElements[3] * static_cast<data_t>(m.mElements[12]) + mElements[7] * static_cast<data_t>(m.mElements[13]) + mElements[11] * static_cast<data_t>(m.mElements[14]) + mElements[15] * static_cast<data_t>(m.mElements[15]));
	}

	/// Post-multiply this matrix by the specified one (this = this * m)
	template <class U>
	Matrix4<data_t> &operator*=(const Matrix4<U> &m)
	{
		postmul(m);
		return *this;
	}

	/// Return the vector obtained multiplying this matrix by the specified vector (homogeneous coordinates)
	template <class U>
	const Vector3<U> operator*(const Vector3<U> &vecOther) const
	{
		const double fW = mElements[3] * vecOther.x() + mElements[7] * vecOther.y() + mElements[11] * vecOther.z() + mElements[15];
		return Vector3<U>(
			(mElements[0] * vecOther.x() + mElements[4] * vecOther.y() + mElements[8] * vecOther.z() + mElements[12]) / fW,
			(mElements[1] * vecOther.x() + mElements[5] * vecOther.y() + mElements[9] * vecOther.z() + mElements[13]) / fW,
			(mElements[2] * vecOther.x() + mElements[6] * vecOther.y() + mElements[10] * vecOther.z() + mElements[14]) / fW);
	}

	/// Return true if two matrices are identical.
	template <class U>
	const bool operator==(const Matrix4<U> &m) const
	{
		for (unsigned int i = 0; i < 16; i++)
		{
			if (mElements[i] != m.mElements[i])
				return false;
		}
		return true;
	}

	/// Return true if two matrices have at least a different element
	template <class U>
	const bool operator!=(const Matrix4<U> &matOther) const
	{
		return !(*this == matOther);
	}

	/// Write out this matrix on the specified stream
	void print(std::ostream &out) const
	{
		out << mElements[0] << " " << mElements[4] << " " << mElements[8] << " " << mElements[12] << "\n"
			<< mElements[1] << " " << mElements[5] << " " << mElements[9] << " " << mElements[13] << "\n"
			<< mElements[2] << " " << mElements[6] << " " << mElements[10] << " " << mElements[14] << "\n"
			<< mElements[3] << " " << mElements[7] << " " << mElements[11] << " " << mElements[15] << std::endl;
	}

	// ********************************************************************************************
	// *** Class members **************************************************************************
private:
	data_t mElements[16];

}; /* Matrix4 */

// ************************************************************************************************
// *** Specializations ****************************************************************************
/// Matrix 4x4 of integers
typedef Matrix4<int> Matrix4i;

/// Matrix 4x4 of floats
typedef Matrix4<float> Matrix4f;

/// Matrix 4x4 of doubles
typedef Matrix4<double> Matrix4d;

// ************************************************************************************************
// *** Implementation *****************************************************************************

#endif /* __MATRIX_H__ */
