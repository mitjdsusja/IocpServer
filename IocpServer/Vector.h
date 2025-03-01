#pragma once
class Vector{
public:
	Vector(float x, float y);
	Vector(float x, float y, float z);

	Vector operator+(const Vector& other) {
		return Vector(_x + other._x, _y + other._y, _z + other._z);
	}
	Vector operator-(const Vector& other) {
		return Vector(_x - other._x, _y - other._y, _z - other._z);
	}

public:
	float _x = 0;
	float _y = 0;
	float _z = 0;

};

