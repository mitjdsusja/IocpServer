#pragma once

template<typename T>
class Vector {
public:
	Vector() {

	}
	Vector(T x, T y) : Vector(x, y, 0) {

	}
	Vector(T x, T y, T z) : _x(x), _y(y), _z(z) {

	}

	Vector& operator+(const Vector& other) {
		return Vector(_x + other._x, _y + other._y, _z + other._z);
	}
	Vector& operator-(const Vector& other) {
		return Vector(_x - other._x, _y - other._y, _z - other._z);
	}
	bool operator!=(const Vector& other) {

		if (this->_x != other._x) return true;
		if (this->_y != other._y) return true;
		if (this->_z != other._z) return true;

		return false;
	}
	Vector& operator=(const Vector& other) {

		_x = other._x;
		_y = other._y;
		_z = other._z;

		return *this;
	}

	bool IsZero() const {
		if (_x != 0 || _y != 0 || _z != 0) {
			return false;
		}
		return true;
	}

public:
	T _x = 0;
	T _y = 0;
	T _z = 0;

};

