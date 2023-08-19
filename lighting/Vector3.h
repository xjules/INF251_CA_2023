#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include <cassert>
#include <cmath>

/// A vector with 3 components of type T
template <class T>
class Vector3 {
    
	typedef T data_t;

	// ********************************************************************************************************
    // *** Basic methods **************************************************************************************
public:
    /// Default constructor
    Vector3() {
        mElements[0] = data_t(0);
        mElements[1] = data_t(0);
        mElements[2] = data_t(0);
    }

    /// Create a vector with the specified values
	template <class U>
    Vector3(const U& fX, const U& fY, const U& fZ) {
        mElements[0] = static_cast<data_t>(fX);
        mElements[1] = static_cast<data_t>(fY);
        mElements[2] = static_cast<data_t>(fZ);
    }

    /// Create a vector with the specified values
	template <class U>
    Vector3(const U* fValues) {
        mElements[0] = static_cast<data_t>(fValues[0]);
        mElements[1] = static_cast<data_t>(fValues[1]);
        mElements[2] = static_cast<data_t>(fValues[2]);
    }

    /// Copy constructor
	template <class U> 
    Vector3(const Vector3<U>& other) {
        mElements[0] = static_cast<data_t>(other.get()[0]);
        mElements[1] = static_cast<data_t>(other.get()[1]);
        mElements[2] = static_cast<data_t>(other.get()[2]);
    }

    /// Assignment operator
	template <class U>
    Vector3<data_t>& operator=(const Vector3<U>& other) {
        if(&other != this) {
			mElements[0] = static_cast<data_t>(other.get()[0]);
			mElements[1] = static_cast<data_t>(other.get()[1]);
			mElements[2] = static_cast<data_t>(other.get()[2]);
        }
        return *this;
    }

    /// Destructor
    ~Vector3() {}


    // ********************************************************************************************************
    // *** Public methods *************************************************************************************
public:
    /// Set the values of the vector
	template <class U>
    void set(const U& fX, const U&fY, const U&fZ) {
		mElements[0] = static_cast<data_t>(fX);
		mElements[1] = static_cast<data_t>(fY);
		mElements[2] = static_cast<data_t>(fZ);
    }

    /// Return a pointer to the elements of the vector
	data_t * get() {
		return mElements;
	}
	const data_t * get() const {
        return mElements;
    }

    /// Return a reference to the i-th element of the vector
	data_t& get(unsigned int i) {
		assert(i < 3);
		return mElements[i];
	}
	const data_t& get(unsigned int i) const {
        assert(i < 3);
        return mElements[i];
    }

    /// Return a reference to the first element of the vector
    data_t& x() {
        return mElements[0];
    }
	const data_t& x() const {
		return mElements[0];
	}

    /// Return the second element of the vector
	data_t& y() {
		return mElements[1];
	}
	const data_t& y() const {
		return mElements[1];
	}

    /// Return the third element of the vector
	data_t& z() {
		return mElements[2];
	}
	const data_t& z() const {
		return mElements[2];
	}

    /// Return the magnitude of the vector
	float magnitude() const {
		return std::sqrt(static_cast<float>(x()*x() + y()*y() + z()*z()));
	}
	double magnituded() const {
		return std::sqrt(static_cast<double>(x()*x() + y()*y() + z()*z()));
	}

    /// Return the square of the magnitude of the vector
	float magnitudeSquared() const {
		return static_cast<float>(x()*x() + y()*y() + z()*z());
	}
	double magnitudeSquaredd() const {
		return static_cast<double>(x()*x() + y()*y() + z()*z());
	}

    /// Return the dot product of this vector with the specified one
	template <class U>
    data_t dot(const Vector3<U>& other) const {
        return (x() * static_cast<data_t>(other.x()) +
			y() * static_cast<data_t>(other.y()) + 
			z() * static_cast<data_t>(other.z()));
    }

    /// Return the cross product of this vector with the specified one
	template <class U>
    Vector3 cross(const Vector3<U> &other) const {
        return Vector3(
			y() * static_cast<data_t>(other.z()) - z() * static_cast<data_t>(other.y()), 
			z() * static_cast<data_t>(other.x()) - x() * static_cast<data_t>(other.z()),
			x() * static_cast<data_t>(other.y()) - y() * static_cast<data_t>(other.x()));
    }

    /// Return a normalized copy of this vector
    Vector3<data_t> getNormalized() const {
        Vector3<data_t> vecNew(*this);
        double magnitude = vecNew.magnituded();

        if (magnitude > 0.0)
            vecNew /= magnitude;
		return vecNew;
    }

    /// Normalize this vector
    void normalize() {
        (*this) = getNormalized();
    }

    /// Return the inverse of this vector
    Vector3<data_t> getInverse() const {
        return Vector3(data_t(1)/x(), data_t(1)/y(), data_t(1)/z());
    }

    /// Invert this vector
    void invert() {
        (*this) = getInverse();
    }

    /// Return the opposite of this vector
    Vector3 getOpposite() const {
        return -(*this); // call operator-
    }

    /// Change this vector in its opposite
    void oppose() {
        (*this) = getOpposite();
    }

    /// Return true if this vector is equal to the specified one
	template <class U>
    bool operator==(const Vector3<U>& other) const {
        return (x() == static_cast<data_t>(other.x())) && 
			(y() == static_cast<data_t>(other.y())) && 
			(z() == static_cast<data_t>(other.z()));
    }

    /// Return true if this vector is different from the specified one
	template <class U>
    bool operator!=(const Vector3<U>& other) const {
        return !(*this == other);
    }

    /// Access the i-th element of the vector
    data_t& operator[](unsigned int i) {
        assert(i < 3);
        return mElements[i];
    }
	const data_t& operator[](unsigned int i) const {
        assert(i<3);
        return mElements[i];
    }

    /// In-place vector sum
	template <class U>
    const Vector3<data_t>& operator+=(const Vector3<U>& other) {
        x() = x() + static_cast<data_t>(other.x());
        y() = y() + static_cast<data_t>(other.y());
        z() = z() + static_cast<data_t>(other.z());
        return *this;
    }

    /// In-place vector difference
	template <class U>
	const Vector3<data_t>& operator-=(const Vector3<U>& other) {
		x() = x() - static_cast<data_t>(other.x());
		y() = y() - static_cast<data_t>(other.y());
		z() = z() - static_cast<data_t>(other.z());
		return *this;
	}


    /// In-place element by element multiplication
	template <class U>
	const Vector3<data_t>& operator*=(const Vector3<U>& other) {
		x() = x() * static_cast<data_t>(other.x());
		y() = y() * static_cast<data_t>(other.y());
		z() = z() * static_cast<data_t>(other.z());
		return *this;
	}
	
	/// In-place scalar multiplication
	template <class U>
	const Vector3<data_t>& operator*=(const U& f) {
		x() = x() * static_cast<data_t>(f);
		y() = y() * static_cast<data_t>(f);
		z() = z() * static_cast<data_t>(f);
		return *this;
	}

	/// In-place element by element division
	template <class U>
    const Vector3<data_t>& operator/=(const Vector3<U>& other) {
        assert(static_cast<data_t>(other.x()) != data_t(0));
        assert(static_cast<data_t>(other.y()) != data_t(0));
        assert(static_cast<data_t>(other.z()) != data_t(0));
        x() = x() / static_cast<data_t>(other.x());
		y() = y() / static_cast<data_t>(other.y());
		z() = z() / static_cast<data_t>(other.z());
        return *this;
    }

     /// In-place scalar division
	template <class U>
    const Vector3<data_t>& operator/=(const U& f) {
        assert(f != 0.);
		x() = x() * static_cast<data_t>(f);
		y() = y() * static_cast<data_t>(f);
		z() = z() * static_cast<data_t>(f);
        return *this;
    }

    /// Vector sum
	template <class U>
    Vector3<data_t> operator+(const Vector3<U>& other) const {
        Vector3<data_t> vecNew(*this);
        vecNew += other;
        return vecNew;
    }

    /// Vector difference
	template <class U>
	Vector3<data_t> operator-(const Vector3<U>& other) const {
		Vector3<data_t> vecNew(*this);
		vecNew -= other;
		return vecNew;
	}

    /// Element by element multiplication
	template <class U>
	Vector3<data_t> operator*(const Vector3<U>& other) const {
		Vector3<data_t> vecNew(*this);
		vecNew *= other;
		return vecNew;
	}

    /// Scalar multiplication
	template <class U>
	Vector3<data_t> operator*(const U& f) const {
		Vector3<data_t> vecNew(*this);
		vecNew *= f;
		return vecNew;
	}

    /// Element by element division
	template <class U>
	Vector3<data_t> operator/(const Vector3<U>& other) const {
		Vector3<data_t> vecNew(*this);
		vecNew /= other;
		return vecNew;
	}

    /// Scalar division
	template <class U>
	Vector3<data_t> operator/(const U& f) const {
		Vector3<data_t> vecNew(*this);
		vecNew /= f;
		return vecNew;
	}

    /// unary minus (return the opposed vector)
    Vector3<data_t> operator-() const {
        return Vector3(-x(), -y(), -z());
    }

    // ********************************************************************************************
    // *** Class members **************************************************************************
private:
    data_t mElements[3];

}; /* Vector */


// ************************************************************************************************
// *** Specializations ****************************************************************************
/// Vector3 of integers
typedef Vector3<int> Vector3i;

/// Vector3 of floats
typedef Vector3<float> Vector3f;

/// Vector3 of doubles
typedef Vector3<double> Vector3d;

#endif /* __VECTOR3_H__ */
