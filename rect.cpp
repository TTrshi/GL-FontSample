#include "rect.h"
//#include <iostream>
#include <cstdlib>
#include <math.h>

using namespace GM;

#define FLT_EPSILON      1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0

bool Vec2::equals(const Vec2& target) const
{
	return (this->x == target.x) ? true : false
		&& (this->y == target.y) ? true : false;
}
const Vec2 Vec2::ZERO = Vec2(0, 0);


bool Vec3::equals(const Vec3& target) const
{
	return (this->x == target.x) ? true : false
		&& (this->y == target.y) ? true : false
		&& (this->z == target.z) ? true : false;
}
const Vec3 Vec3::ZERO = Vec3(0, 0, 0);

bool Vec4::equals(const Vec4& target) const
{
	return (this->w == target.w) ? true : false
		&& (this->x == target.x) ? true : false
		&& (this->y == target.y) ? true : false
		&& (this->z == target.z) ? true : false;
}
const Vec4 Vec4::ZERO = Vec4(0,0, 0, 0);


Size& Size::operator= (const Size& other)
{
	setSize(other.width, other.height);
	return *this;
}

Size& Size::operator= (const Vec2& point)
{
	setSize(point.x, point.y);
	return *this;
}

Size Size::operator+(const Size& right) const
{
	return Size(this->width + right.width, this->height + right.height);
}

Size Size::operator-(const Size& right) const
{
	return Size(this->width - right.width, this->height - right.height);
}

Size Size::operator*(float a) const
{
	return Size(this->width * a, this->height * a);
}

Size Size::operator/(float a) const
{
	//CCASSERT(a != 0, "CCSize division by 0.");
	return Size(this->width / a, this->height / a);
}

void Size::setSize(float w, float h)
{
	this->width = w;
	this->height = h;
}

bool Size::equals(const Size& target) const
{
	return (abs(this->width - target.width)  < FLT_EPSILON)
		&& (abs(this->height - target.height) < FLT_EPSILON);
}

const Size Size::ZERO = Size(0, 0);

Rect& Rect::operator= (const Rect& other)
{
	setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
	return *this;
}

bool Rect::equals(const Rect& rect) const
{
	return (origin.equals(rect.origin) &&
		size.equals(rect.size));
}

float Rect::getMaxX() const
{
	return origin.x + size.width;
}

float Rect::getMidX() const
{
	return origin.x + size.width / 2.0f;
}

float Rect::getMinX() const
{
	return origin.x;
}

float Rect::getMaxY() const
{
	return origin.y + size.height;
}

float Rect::getMidY() const
{
	return origin.y + size.height / 2.0f;
}

float Rect::getMinY() const
{
	return origin.y;
}

bool Rect::containsPoint(const Vec2& point) const
{
	bool bRet = false;

	if (point.x >= getMinX() && point.x <= getMaxX()
		&& point.y >= getMinY() && point.y <= getMaxY())
	{
		bRet = true;
	}

	return bRet;
}

bool Rect::intersectsRect(const Rect& rect) const
{
	return !(getMaxX() < rect.getMinX() ||
		rect.getMaxX() <      getMinX() ||
		getMaxY() < rect.getMinY() ||
		rect.getMaxY() <      getMinY());
}

bool Rect::intersectsCircle(const Vec2& center, float radius) const
{
	Vec2 rectangleCenter((origin.x + size.width / 2),
		(origin.y + size.height / 2));

	float w = size.width / 2;
	float h = size.height / 2;

	float dx = abs(center.x - rectangleCenter.x);
	float dy = abs(center.y - rectangleCenter.y);

	if (dx > (radius + w) || dy > (radius + h))
	{
		return false;
	}

	Vec2 circleDistance(abs(center.x - origin.x - w),
		abs(center.y - origin.y - h));

	if (circleDistance.x <= (w))
	{
		return true;
	}

	if (circleDistance.y <= (h))
	{
		return true;
	}

	float cornerDistanceSq = powf(circleDistance.x - w, 2) + powf(circleDistance.y - h, 2);

	return (cornerDistanceSq <= (powf(radius, 2)));
}

/*
void Rect::merge(const Rect& rect)
{
	float minX = std::min(getMinX(), rect.getMinX());
	float minY = std::min(getMinY(), rect.getMinY());
	float maxX = std::max(getMaxX(), rect.getMaxX());
	float maxY = std::max(getMaxY(), rect.getMaxY());
	setRect(minX, minY, maxX - minX, maxY - minY);
}
*/

/*
Rect Rect::unionWithRect(const Rect & rect) const
{
	float thisLeftX = origin.x;
	float thisRightX = origin.x + size.width;
	float thisTopY = origin.y + size.height;
	float thisBottomY = origin.y;

	if (thisRightX < thisLeftX)
	{
		std::swap(thisRightX, thisLeftX);   // This rect has negative width
	}

	if (thisTopY < thisBottomY)
	{
		std::swap(thisTopY, thisBottomY);   // This rect has negative height
	}

	float otherLeftX = rect.origin.x;
	float otherRightX = rect.origin.x + rect.size.width;
	float otherTopY = rect.origin.y + rect.size.height;
	float otherBottomY = rect.origin.y;

	if (otherRightX < otherLeftX)
	{
		std::swap(otherRightX, otherLeftX);   // Other rect has negative width
	}

	if (otherTopY < otherBottomY)
	{
		std::swap(otherTopY, otherBottomY);   // Other rect has negative height
	}

	float combinedLeftX = std::min(thisLeftX, otherLeftX);
	float combinedRightX = std::max(thisRightX, otherRightX);
	float combinedTopY = std::max(thisTopY, otherTopY);
	float combinedBottomY = std::min(thisBottomY, otherBottomY);

	return Rect(combinedLeftX, combinedBottomY, combinedRightX - combinedLeftX, combinedTopY - combinedBottomY);
}
*/

const Rect Rect::ZERO = Rect(0, 0, 0, 0);
