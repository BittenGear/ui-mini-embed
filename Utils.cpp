#pragma once

namespace UIMiniEmbed {

	float clamp(const float min, float val, const float max) {
		val = ( min <= val ) ? val : min;
		val = ( val <= max ) ? val : max;
		return val;
	}
	double clampDouble(const double min, double val, const double max) {
		val = ( min <= val ) ? val : min;
		val = ( val <= max ) ? val : max;
		return val;
	}
	float normalizeFloat(const float val, const float def = 0) {
		if ( val == val )
			return val;
		return def;
	}

	struct Vec2 {
		float x = 0;
		float y = 0;
		
		std::string dump() const { return "Vec2{" + std::to_string(x) + ", " + std::to_string(y) + "}"; }
	};
	Vec2 operator+(const Vec2& a, const Vec2& b) { return { a.x + b.x, a.y + b.y }; }
	Vec2 operator-(const Vec2& a, const Vec2& b) { return { a.x - b.x, a.y - b.y }; }
	Vec2 operator*(const Vec2& a, const Vec2& b) { return { a.x * b.x, a.y * b.y }; }
	Vec2 operator/(const Vec2& a, const Vec2& b) { return { a.x / b.x, a.y / b.y }; }
	
	struct BBox {
		Vec2 min;
		Vec2 max;

		float getWidth () const { return max.x - min.x; }
		float getHeight() const { return max.y - min.y; }
		
		bool  isPointOnBBox(const Vec2& p) const {
			return ( min.x <= p.x ) && ( p.x <= max.x ) && ( min.y <= p.y ) && ( p.y <= max.y );
		}
		
		std::string dump() const { return "BBox{" + min.dump() + ", " + max.dump() + "}"; }
	};

	template< class T >
	T stringToNumber(const std::string& val, const T defValue = 0) {
		T value = defValue;
		std::istringstream is(val);
		is >> value;
		if ( is.fail() )
			value = defValue;
		return value;
	}
	float stringToFloat(const std::string& val, const float defValue = 0) {
		return stringToNumber< float >(val, defValue);
	}
	
}
