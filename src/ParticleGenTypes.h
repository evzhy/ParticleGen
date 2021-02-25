#pragma once

const int coordinatesPerVertex = 3;

namespace gen
{
	typedef struct alignas( float )
	{
		float x{0.f}, y{0.f}, z{0.f};
	} Vec3;
}
