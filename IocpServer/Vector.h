#pragma once

template<typename T>
class Vector{
public:
	Vector() {

	}
	Vector(T x, T y) : Vector(x, y, 0) {

	}
	Vector(T x, T y, T z) : _x(x), _y(y), _z(z) {

	}

	Vector operator+(const Vector& other) {
		return Vector(_x + other._x, _y + other._y, _z + other._z);
	}
	Vector operator-(const Vector& other) {
		return Vector(_x - other._x, _y - other._y, _z - other._z);
	}

public:
	T _x = 0;
	T _y = 0;
	T _z = 0;

};

