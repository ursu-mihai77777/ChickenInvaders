#pragma once
#include "Vec2.h"

class CBoundingBox
{
public:
	Vec2 min;
	Vec2 max;

	CBoundingBox(Vec2 min, Vec2 max)
	{
		this->min = min;
		this->max = max;
	}

	Vec2 GetCenter()
	{
		return (min + max) / 2;
	}

	bool Contains(Vec2& point)
	{
		if ((point.x >= min.x && point.x <= max.x) && (point.y >= min.y && point.y <= max.y))
			return true;
		return false;
	}

	bool Intersects(CBoundingBox& box)
	{
		if (Contains(box.min) || Contains(box.max) || Contains(Vec2(box.min.x, box.max.y)) || Contains(Vec2(box.max.x, box.min.y))
			|| box.Contains(min) || box.Contains(max))
			return true;
		return false;
	}
};

