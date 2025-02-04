#pragma once

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Includes                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// Std-Includes
#include <cmath>

// Other Includes

// Internal Includes
#include "_global.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Class                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ENGINE_NAMESPACE_BEGIN

#define VECTOR4F_COMPONENTS 4
#define VECTOR4F_BYTES      VECTOR4F_COMPONENTS * FLOAT_BYTES

struct vec4 {
    float x;
    float y;
    float z;
    float w;
};

class Vector4f
{
public:

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                     Public Static                      */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    static Vector4f X_AXIS;
    static Vector4f Y_AXIS;
    static Vector4f Z_AXIS;
    static Vector4f W_AXIS;


    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Public                          */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            Vector4f();
            Vector4f(const float x, const float y);
            Vector4f(const float x, const float y, const float z);
            Vector4f(const float x, const float y, const float z, const float w);
            ~Vector4f() = default;

    bool     is_unit();
    bool     is_zero();
    bool operator==(const Vector4f& o) const;
    bool operator!=(const Vector4f& o) const;

    Vector4f operator-() const;

    Vector4f operator+(const Vector4f o) const;
    Vector4f operator-(const Vector4f o) const;
    Vector4f operator*(const Vector4f o) const;
    Vector4f operator/(const Vector4f o) const;

    Vector4f& operator+=(const Vector4f o);
    Vector4f& operator-=(const Vector4f o);
    Vector4f& operator*=(const Vector4f o);
    Vector4f& operator/=(const Vector4f o);

    Vector4f operator+(const float o) const;
    Vector4f operator-(const float o) const;
    Vector4f operator*(const float o) const;
    Vector4f operator/(const float o) const;

    Vector4f& operator+=(const float o);
    Vector4f& operator-=(const float o);
    Vector4f& operator*=(const float o);
    Vector4f& operator/=(const float o);

    Vector4f normalized() const;
    float    length() const;

    vec4     toVec4() const;

    float x;
    float y;
    float z;
    float w;
private:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Private                         */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

};

inline void write_into( std::vector<float>& vector, Vector4f value ) 
{
    vector.insert( vector.end(), value.x );
    vector.insert( vector.end(), value.y );
    vector.insert( vector.end(), value.z );
    vector.insert( vector.end(), value.w );
}

ENGINE_NAMESPACE_END
