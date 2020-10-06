#pragma once
#include "Vector3Mixin.h"
#include <cstdint>

template<typename T>
struct Vector3Base
{
	T x;
	T y;
	T z;

	constexpr Vector3Base() = default;
	constexpr Vector3Base(T x, T y, T z):x(x),y(y),z(z){};
};

template<typename T>
using Vec3 = Vector3Mixin<Vector3Base<T>, &Vector3Base<T>::x, &Vector3Base<T>::y, &Vector3Base<T>::z>;
using Vec3f = Vec3<float>;
using Vec3u8 = Vec3<uint8_t>;