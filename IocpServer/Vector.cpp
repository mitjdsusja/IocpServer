#include "pch.h"
#include "Vector.h"

Vector::Vector() {

}

Vector::Vector(float x, float y) : Vector(x, y, 0){

}

Vector::Vector(float x, float y, float z) : _x(x), _y(y), _z(z){

}
