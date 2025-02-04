#pragma once

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Includes                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// Std-Includes
#include <cmath>

// Other Includes

// Internal Includes
#include "_global.h"
#include "vector4f.h"
#include "value.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Class                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ENGINE_NAMESPACE_BEGIN

#define VIEWPORT4I_COMPONENTS 4
#define VIEWPORT4I_BYTES      VIEWPORT4I_COMPONENTS * INT32_BYTES

template<class TYPE>
struct Viewport4 : Value
{
    static_assert(std::is_same<TYPE, float>::value
               || std::is_same<TYPE, int32>::value, "TYPE must be float or int32");


            Viewport4();
            Viewport4(const TYPE x, const TYPE y);
            Viewport4(const TYPE x, const TYPE y, const TYPE w);
            Viewport4(const TYPE x, const TYPE y, const TYPE w, const TYPE h);
            ~Viewport4() = default;

    bool operator==(const Viewport4& o) const;
    bool operator!=(const Viewport4& o) const;

    Viewport4 operator+(const Viewport4 o) const;
    Viewport4 operator-(const Viewport4 o) const;

    Viewport4& operator+=(const Viewport4 o);
    Viewport4& operator-=(const Viewport4 o);

    TYPE x;
    TYPE y;
    TYPE h;
    TYPE w;
};

typedef Viewport4<int32> Viewport4i;
typedef Viewport4<float> Viewport4f;

template<class TYPE>
Viewport4<TYPE>::Viewport4() : x(0), y(0), w(0), h(0) { }

template<class TYPE>
Viewport4<TYPE>::Viewport4(const TYPE x, const TYPE y) : x(x), y(y), w(0), h(0) { }

template<class TYPE>
Viewport4<TYPE>::Viewport4(const TYPE x, const TYPE y, const TYPE w) : x(x), y(y), w(w), h(0) { }

template<class TYPE>
Viewport4<TYPE>::Viewport4(const TYPE x, const TYPE y, const TYPE w, const TYPE h) : x(x), y(y), w(w), h(h) { }

template<class TYPE>
bool Viewport4<TYPE>::operator==(const Viewport4& o) const
{
    return x == o.x
        && y == o.y
        && w == o.w
        && h == o.h;
}

template<class TYPE>
bool Viewport4<TYPE>::operator!=(const Viewport4<TYPE> & o) const
{
    return !(*this == o);
}

template<class TYPE>
Viewport4<TYPE> Viewport4<TYPE>::operator+(const Viewport4<TYPE> o) const
{
    Viewport4 result = *this;
    result += o;
    return result;
}

template<class TYPE>
Viewport4<TYPE> Viewport4<TYPE>::operator-(const Viewport4<TYPE> o) const
{
    Viewport4 result = *this;
    result -= o;
    return result;
}

template<class TYPE>
Viewport4<TYPE>& Viewport4<TYPE>::operator+=(const Viewport4<TYPE> o)
{
    x += o.x;
    y += o.y;
    w += o.w;
    h += o.h;
    return *this;
}

template<class TYPE>
Viewport4<TYPE>& Viewport4<TYPE>::operator-=(const Viewport4<TYPE> o)
{
    x -= o.x;
    y -= o.y;
    w -= o.w;
    h -= o.h;
    return *this;
}

ENGINE_NAMESPACE_END
