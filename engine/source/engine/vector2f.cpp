#include "stdafx.h"
#include "vector2f.h"

ENGINE_NAMESPACE_BEGIN

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                      Public Static                     */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Vector2f Vector2f::X_AXIS = Vector2f(1.0f, 0.0f);
Vector2f Vector2f::Y_AXIS = Vector2f(0.0f, 1.0f);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Public                         */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Vector2f::Vector2f() : x(0), y(0) {}

Vector2f::Vector2f( const Vector2f& copy ) : x(copy.x), y(copy.y) {}

Vector2f::Vector2f(const float x, const float y) : x(x), y(y) {}

bool Vector2f::is_unit()
{
    return x == 1.0f && y == 1.0f;
}

bool Vector2f::is_zero()
{
    return x == 0.0f && y == 0.0f;
}

bool Vector2f::operator==(const Vector2f& o) const
{
    return x == o.x
        && y == o.y;
}

bool Vector2f::operator!=(const Vector2f & o) const
{
    return !(*this == o);
}

Vector2f Vector2f::operator-() const
{
    Vector2f result;
    result.x = -x;
    result.y = -y;
    return result;
}

Vector2f Vector2f::operator+(const Vector2f o) const
{
    Vector2f result = *this;
    result += o;
    return result;
}

Vector2f Vector2f::operator-(const Vector2f o) const
{
    Vector2f result = *this;
    result -= o;
    return result;
}

Vector2f Vector2f::operator*(const Vector2f o) const
{
    Vector2f result = *this;
    result *= o;
    return result;
}

Vector2f Vector2f::operator/(const Vector2f o) const
{
    Vector2f result = *this;
    result /= o;
    return result;
}


Vector2f& Vector2f::operator+=(const Vector2f o)
{
    x += o.x;
    y += o.y;
    return *this;
}

Vector2f& Vector2f::operator-=(const Vector2f o)
{
    x -= o.x;
    y -= o.y;
    return *this;
}

Vector2f& Vector2f::operator*=(const Vector2f o)
{
    x *= o.x;
    y *= o.y;
    return *this;
}

Vector2f& Vector2f::operator/=(const Vector2f o)
{
    x /= o.x;
    y /= o.y;
    return *this;
}


Vector2f Vector2f::operator+(const float o) const
{
    Vector2f result = *this;
    result += o;
    return result;
}

Vector2f Vector2f::operator-(const float o) const
{
    Vector2f result = *this;
    result -= o;
    return result;
}

Vector2f Vector2f::operator*(const float o) const
{
    Vector2f result = *this;
    result *= o;
    return result;
}

Vector2f Vector2f::operator/(const float o) const
{
    Vector2f result = *this;
    result /= o;
    return result;
}


Vector2f& Vector2f::operator+=(const float o)
{
    x += o;
    y += o;
    return *this;
}

Vector2f& Vector2f::operator-=(const float o)
{
    x -= o;
    y -= o;
    return *this;
}

Vector2f& Vector2f::operator*=(const float o)
{
    x *= o;
    y *= o;
    return *this;
}

Vector2f& Vector2f::operator/=(const float o)
{
    x /= o;
    y /= o;
    return *this;
}

Vector2f Vector2f::normalized() const
{
    Vector2f result = *this;
    float len = result.length();

    if ( len != 0.0f ) {
        result.x = result.x / len;
        result.y = result.y / len;
    }

    return result;
}

float Vector2f::length() const
{
    return std::sqrt( x*x + y*y);
}

float Vector2f::cross( Vector2f & o )
{
    return x * o.y - o.x * y;
}

float Vector2f::dot( Vector2f& o )
{
    return x * o.x + y * o.y;
}

ENGINE_NAMESPACE_END

