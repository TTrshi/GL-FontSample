#ifndef __RECT_H__
#define __RECT_H__

namespace GM {

	class  Vec2
	{
	public:
		float x;
		float y;
	public:
		Vec2() { x = y = 0; }
		Vec2(float _x, float _y) { x = _x; y = _y; }
		bool equals(const Vec2& target) const;
		static const Vec2 ZERO;

		Vec2 operator+(float a) const { return Vec2(x + a, y + a); }
		Vec2 operator-(float a) const { return Vec2(x - a, y - a); }
		Vec2 operator*(float a) const { return Vec2(x * a, y * a); }
		Vec2 operator/(float a) const { return Vec2(x / a, y / a); }
		void operator+=(float a) { x += a; y += a; }
		void operator-=(float a) { x -= a; y -= a; }
		void operator*=(float a) { x *= a; y *= a; }
		void operator/=(float a) { x /= a; y /= a; }

		Vec2 operator+(const Vec2& _vec) const { return Vec2(x + _vec.x, y + _vec.y); }
		Vec2 operator-(const Vec2& _vec) const { return Vec2(x - _vec.x, y - _vec.y); }
		Vec2 operator*(const Vec2& _vec) const { return Vec2(x * _vec.x, y * _vec.y); }
		Vec2 operator/(const Vec2& _vec) const { return Vec2(x / _vec.x, y / _vec.y); }
		void operator+=(const Vec2& _vec) { x += _vec.x; y += _vec.y; }
		void operator-=(const Vec2& _vec) { x -= _vec.x; y -= _vec.y; }
		void operator*=(const Vec2& _vec) { x *= _vec.x; y *= _vec.y; }
		void operator/=(const Vec2& _vec) { x /= _vec.x; y /= _vec.y; }
	};

	class  Vec3
	{
	public:
		float x;
		float y;
		float z;
	public:
		Vec3() { x = y = z = 0; }
		Vec3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
		bool equals(const Vec3& target) const;
		static const Vec3 ZERO;

		Vec3 operator+(float a) const { return Vec3(x + a, y + a, z + a); }
		Vec3 operator-(float a) const { return Vec3(x - a, y - a, z - a); }
		Vec3 operator*(float a) const { return Vec3(x * a, y * a, z * a); }
		Vec3 operator/(float a) const { return Vec3(x / a, y / a, z / a); }
		void operator+=(float a) { x += a; y += a; z += a; }
		void operator-=(float a) { x -= a; y -= a; z -= a; }
		void operator*=(float a) { x *= a; y *= a; z *= a; }
		void operator/=(float a) { x /= a; y /= a; z /= a; }

		Vec3 operator+(const Vec3& _vec) const { return Vec3(x + _vec.x, y + _vec.y, z + _vec.z); }
		Vec3 operator-(const Vec3& _vec) const { return Vec3(x - _vec.x, y - _vec.y, z - _vec.z); }
		Vec3 operator*(const Vec3& _vec) const { return Vec3(x * _vec.x, y * _vec.y, z * _vec.z); }
		Vec3 operator/(const Vec3& _vec) const { return Vec3(x / _vec.x, y / _vec.y, z / _vec.z); }
		void operator+=(const Vec3& _vec) { x += _vec.x; y += _vec.y;  z += _vec.z; }
		void operator-=(const Vec3& _vec) { x -= _vec.x; y -= _vec.y;  z -= _vec.z; }
		void operator*=(const Vec3& _vec) { x *= _vec.x; y *= _vec.y;  z *= _vec.z; }
		void operator/=(const Vec3& _vec) { x /= _vec.x; y /= _vec.y;  z /= _vec.z; }
	};

	class  Vec4
	{
	public:
		float w;
		float x;
		float y;
		float z;
	public:
		Vec4() { w = x = y = z = 0; }
		Vec4(float _w, float _x, float _y, float _z) { w = _w; x = _x; y = _y; z = _z; }
		bool equals(const Vec4& target) const;
		static const Vec4 ZERO;

		Vec4 operator+(float a) const { return Vec4(w + a, x + a, y + a, z + a); }
		Vec4 operator-(float a) const { return Vec4(w - a, x - a, y - a, z - a); }
		Vec4 operator*(float a) const { return Vec4(w * a, x * a, y * a, z * a); }
		Vec4 operator/(float a) const { return Vec4(w / a, x / a, y / a, z / a); }
		void operator+=(float a) { w += a; x += a; y += a; z += a; }
		void operator-=(float a) { w -= a; x -= a; y -= a; z -= a; }
		void operator*=(float a) { w *= a; x *= a; y *= a; z *= a; }
		void operator/=(float a) { w /= a; x /= a; y /= a; z /= a; }

		Vec4 operator+(const Vec4& _vec) const { return Vec4(w + _vec.x, x + _vec.x, y + _vec.y, z + _vec.z); }
		Vec4 operator-(const Vec4& _vec) const { return Vec4(w - _vec.x, x - _vec.x, y - _vec.y, z - _vec.z); }
		Vec4 operator*(const Vec4& _vec) const { return Vec4(w * _vec.x, x * _vec.x, y * _vec.y, z * _vec.z); }
		Vec4 operator/(const Vec4& _vec) const { return Vec4(w / _vec.x, x / _vec.x, y / _vec.y, z / _vec.z); }
		void operator+=(const Vec4& _vec) { w += _vec.x; x += _vec.x; y += _vec.y;  z += _vec.z; }
		void operator-=(const Vec4& _vec) { w -= _vec.x; x -= _vec.x; y -= _vec.y;  z -= _vec.z; }
		void operator*=(const Vec4& _vec) { w *= _vec.x; x *= _vec.x; y *= _vec.y;  z *= _vec.z; }
		void operator/=(const Vec4& _vec) { w /= _vec.x; x /= _vec.x; y /= _vec.y;  z /= _vec.z; }
	};


	class  Size
	{
	public:
		/**Width of the Size.*/
		float width;
		/**Height of the Size.*/
		float height;
	public:
		/**Conversion from Vec2 to Size.*/
		operator Vec2() const
		{
			return Vec2(width, height);
		}

	public:
		/**
		@{
		Constructor.
		@param width Width of the size.
		@param height Height of the size.
		@param other Copy constructor.
		@param point Conversion from a point.
		*/
		Size() : width(0), height(0) {}
		Size(float w, float h) : width(w), height(h){}
		Size(const Size& other) : width(other.width), height(other.height){}
		//explicit Size(const Vec2& point);
		/**@}*/

		/**
		* @js NA
		* @lua NA
		*/
		Size& operator= (const Size& other);
		/**
		* @js NA
		* @lua NA
		*/
		Size& operator= (const Vec2& point);
		/**
		* @js NA
		* @lua NA
		*/
		Size operator+(const Size& right) const;
		/**
		* @js NA
		* @lua NA
		*/
		Size operator-(const Size& right) const;
		/**
		* @js NA
		* @lua NA
		*/
		Size operator*(float a) const;
		/**
		* @js NA
		* @lua NA
		*/
		Size operator/(float a) const;
		/**
		Set the width and height of Size.
		* @js NA
		* @lua NA
		*/
		void setSize(float width, float height);
		/**
		Check if two size is the same.
		* @js NA
		*/
		bool equals(const Size& target) const;
		/**Size(0,0).*/
		static const Size ZERO;
	};

	/**Rectangle area.*/
	class Rect
	{
	public:
		/**Low left point of rect.*/
		Vec2 origin;
		/**Width and height of the rect.*/
		Size  size;

	public:
		Rect(void){setRect(0.0f, 0.0f, 0.0f, 0.0f);}

		Rect(float x, float y, float width, float height)
		{
			setRect(x, y, width, height);
		}

		Rect(const Vec2& pos, const Size& dimension)
		{
			setRect(pos.x, pos.y, dimension.width, dimension.height);
		}

		Rect(const Rect& other)
		{
			setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
		}
		/**
		* @js NA
		* @lua NA
		*/
		Rect& operator= (const Rect& other);

		void setRect(float x, float y, float width, float height)
		{
			origin.x = x;
			origin.y = y;
			size.width = width;
			size.height = height;
		}
		/**
		Get the left of the rect.
		* @js NA
		*/
		float getMinX() const; /// return the leftmost x-value of current rect
							   /**
							   Get the X coordinate of center point.
							   * @js NA
							   */
		float getMidX() const; /// return the midpoint x-value of current rect
							   /**
							   Get the right of rect.
							   * @js NA
							   */
		float getMaxX() const; /// return the rightmost x-value of current rect
							   /**
							   Get the bottom of rect.
							   * @js NA
							   */
		float getMinY() const; /// return the bottommost y-value of current rect
							   /**
							   Get the Y coordinate of center point.
							   * @js NA
							   */
		float getMidY() const; /// return the midpoint y-value of current rect
							   /**
							   Get top of rect.
							   * @js NA
							   */
		float getMaxY() const; /// return the topmost y-value of current rect
							   /**
							   Compare two rects.
							   * @js NA
							   */
		bool equals(const Rect& rect) const;
		/**
		Check if the points is contained in the rect.
		* @js NA
		*/
		bool containsPoint(const Vec2& point) const;
		/**
		Check the intersect status of two rects.
		* @js NA
		*/
		bool intersectsRect(const Rect& rect) const;
		/**
		Check the intersect status of the rect and a circle.
		* @js NA
		*/
		bool intersectsCircle(const Vec2& center, float radius) const;
		/**
		Get the min rect which can contain this and rect.
		* @js NA
		* @lua NA
		*/
		/*
		Rect unionWithRect(const Rect & rect) const;
		*/
		/**Compute the min rect which can contain this and rect, assign it to this.*/
		/*
		void merge(const Rect& rect);
		*/
		/**An empty Rect.*/
		static const Rect ZERO;
	};
}


#endif