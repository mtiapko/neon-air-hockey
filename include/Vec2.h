#ifndef __VEC2_H__
#define __VEC2_H__

#include <cmath>
#include <iostream>

namespace hockey
{

template<typename T>
class Vec2
{
private:
	T m_x;
	T m_y;

public:
	constexpr Vec2() : m_x(), m_y() {}
	constexpr Vec2(T x, T y) : m_x(x), m_y(y) {}
	template<typename D> constexpr Vec2(const Vec2<D>& that) : m_x(that.x), m_y(that.y) {}

	template<typename D>
	constexpr Vec2& operator=(const Vec2<D>& that)
	{
		this->m_x = that.m_x;
		this->m_y = that.m_y;
		return *this;
	}

	constexpr Vec2 operator-() const
	{
		return { -this->m_x, -this->m_y };
	}

	template<typename D>
	constexpr auto operator+(const Vec2<D>& that) const -> Vec2<decltype(T{} + D{})>
	{
		return { this->m_x + that.m_x, this->m_y + that.m_y };
	}

	template<typename D>
	constexpr auto operator-(const Vec2<D>& that) const -> Vec2<decltype(T{} - D{})>
	{
		return { this->m_x - that.m_x, this->m_y - that.m_y };
	}

	template<typename D>
	constexpr auto operator*(const Vec2<D>& that) const -> Vec2<decltype(T{} * D{})>
	{
		return { this->m_x * that.m_x, this->m_y * that.m_y };
	}

	template<typename D>
	constexpr auto operator/(const Vec2<D>& that) const -> Vec2<decltype(T{} / D{})>
	{
		return { this->m_x / that.m_x, this->m_y / that.m_y };
	}

	template<typename D>
	constexpr Vec2& operator+=(const Vec2<D>& that)
	{
		this->m_x += that.m_x;
		this->m_y += that.m_y;
		return *this;
	}

	template<typename D>
	constexpr Vec2& operator-=(const Vec2<D>& that)
	{
		this->m_x -= that.m_x;
		this->m_y -= that.m_y;
		return *this;
	}

	template<typename D>
	constexpr Vec2& operator*=(const Vec2<D>& that)
	{
		this->m_x *= that.m_x;
		this->m_y *= that.m_y;
		return *this;
	}

	template<typename D>
	constexpr Vec2& operator/=(const Vec2<D>& that)
	{
		this->m_x /= that.m_x;
		this->m_y /= that.m_y;
		return *this;
	}

	template<typename D>
	constexpr Vec2 operator*(D val)
	{
		return { this->m_x * val, this->m_y * val };
	}

	constexpr T x() const { return m_x; }
	constexpr T y() const { return m_y; }

	constexpr void set_x(T x) { this->m_x = x; }
	constexpr void set_y(T y) { this->m_y = y; }

	constexpr float length() const
	{
		return std::sqrt(std::pow(m_x, 2.0f) + std::pow(m_y, 2.0f));
	}

	constexpr float normalize()
	{
		float len = this->length();
		this->m_x /= len;
		this->m_y /= len;
		return len;
	}

	template<typename D>
	constexpr float distance(const Vec2<D>& that) const
	{
		return std::sqrt(
			std::pow(this->m_x - that.m_x, 2.0f) +
			std::pow(this->m_y - that.m_y, 2.0f)
		);
	}

	template<typename D>
	constexpr float dot(const Vec2<D>& that) const
	{
		return (this->m_x * that.m_x) + (this->m_y * that.m_y);
	}

	friend std::ostream& operator<<(std::ostream& out, const Vec2& vec)
	{
		return out << '(' << vec.x() << ", " << vec.y() << ')';
	}
};

using Vec2f = Vec2<float>;

}

#endif  //  !__VEC2_H__
