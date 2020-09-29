#include "Colors.h"

namespace Colors
{

namespace
{
template<class T>
constexpr const T& clamp( T v, T lo, T hi )
{
	return (v < lo) ? lo : (hi < v) ? hi : v;
}
}

Vec3f saturate_hue(float hue)
{
	const Vec3f channels{0.0, 2.0 / 3.0, 1.0 / 3.0}; 
	return Vec3f(
		clamp<float>(std::abs(fmodf(hue + channels.x, 1.0) * 6.0 - 3.0)-1.0, 0.0, 1.0),
		clamp<float>(std::abs(fmodf(hue + channels.y, 1.0) * 6.0 - 3.0)-1.0, 0.0, 1.0),
		clamp<float>(std::abs(fmodf(hue + channels.z, 1.0) * 6.0 - 3.0)-1.0, 0.0, 1.0)
	);
}

float hue(Vec3f color)
{
	float maxV = std::max({color.x, color.y, color.z});
	float minV = std::min({color.x, color.y, color.z});
	float chroma = maxV - minV;
	const float step = 1.0f / 6.0f;
	if(chroma == 0.0f)
		return 0.0f;
	if(color.x == maxV)
		return step * (color.y - color.z) / chroma;
	else if(color.y == maxV)
		return 2.0f / 6.0f + step * (color.z - color.x) / chroma;
	else
		return 4.0f / 6.0f + step * (color.x - color.y) / chroma;
}

float vibrance(LinRGB color)
{
	const float lum = luminance709(color);
	color = color.visit([lum](float val){ return (val<lum)?lum:val;});
	return (color - lum).length(); // Calculate distance
}

}