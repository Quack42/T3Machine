#pragma once

template <typename T>
class Vector3 {
public:
	T x = 0;
	T y = 0;
	T z = 0;

	Vector3() :
			x(0),
			y(0),
			z(0)
	{
	}

	Vector3(T x, T y, T z) :
			x(x),
			y(y),
			z(z)
	{
	}

	Vector3<T> operator-(Vector3<T> & rhs) const {
		return Vector3<T>(	 x-rhs.x,
							 y-rhs.y,
							 z-rhs.z );
	}

	T & getRefX() {
		return x;
	}

	T & getRefY() {
		return y;
	}

	T & getRefZ() {
		return z;
	}
};

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;
