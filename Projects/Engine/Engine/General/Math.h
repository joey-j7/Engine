#pragma once

#include "Engine/Core.h"

#include <glm/glm.hpp>
#include <include/core/SkColor.h>

#include "box2d/b2_math.h"

namespace Engine
{
    class Engine_API Vector2 {
    public:
        union { float x, r, s; };
        union { float y, g, t; };

        Vector2() : x(0.f), y(0.f) {}

        template <typename T>
        Vector2(const T& Val) : x(static_cast<float>(Val)), y(static_cast<float>(Val)) {}

        template <typename T>
        Vector2(const T& xVal, const T& yVal) : x(static_cast<float>(xVal)), y(static_cast<float>(yVal)) {}

        Vector2(const glm::vec2& Vector) : x(Vector.x), y(Vector.y) {}
        Vector2(const b2Vec2& Vector) : x(Vector.x), y(Vector.y) {}

        operator glm::vec2() const { return glm::vec2(x, y); }
        operator b2Vec2() const { return b2Vec2(x, y); }
        
        template<typename T>
        Vector2& operator=(const T& Other)
        {
            *this = Vector2(Other.x, Other.y);
            return *this;
        }

        Vector2& operator=(float Other)
        {
            *this = Vector2(Other);
            return *this;
        }

        template<typename T>
        Vector2& operator+=(const T& Other)
        {
            *this += Vector2(Other.x, Other.y);
            return *this;
        }

        Vector2& operator+=(const Vector2& Other)
        {
            this->x += Other.x;
            this->y += Other.y;
            return *this;
        }

        Vector2& operator+=(float Other)
        {
            *this += Vector2(Other);
            return *this;
        }

        template<typename T>
        Vector2& operator-=(const T& Other)
        {
            *this -= Vector2(Other.x, Other.y);
            return *this;
        }

        Vector2& operator-=(const Vector2& Other)
        {
            this->x -= Other.x;
            this->y -= Other.y;
            return *this;
        }

        Vector2& operator-=(float Other)
        {
            *this -= Vector2(Other);
            return *this;
        }

        template<typename T>
        Vector2& operator*=(const T& Other)
        {
            *this *= Vector2(Other.x, Other.y);
            return *this;
        }

        Vector2& operator*=(const Vector2& Other)
        {
            this->x *= Other.x;
            this->y *= Other.y;
            return *this;
        }

        Vector2& operator*=(float Other)
        {
            *this *= Vector2(Other);
            return *this;
        }

        template<typename T>
        Vector2& operator/=(const T& Other)
        {
            *this /= Vector2(Other.x, Other.y);
            return *this;
        }

        Vector2& operator/=(const Vector2& Other)
        {
            this->x /= Other.x;
            this->y /= Other.y;
            return *this;
        }

        Vector2& operator/=(float Other)
        {
            *this /= Vector2(Other);
            return *this;
        }

        template<typename T>
        bool operator==(const T& Other) const
        {
            return *this == Vector2(Other.x, Other.y);
        }

        bool operator==(const Vector2& Other) const
        {
            return this->x == Other.x &&
                this->y == Other.y;
        }

        bool operator==(float Other) const
        {
            return *this == Vector2(Other);
        }

        template<typename T>
        bool operator!=(const T& Other) const
        {
            return *this == Vector2(Other.x, Other.y);
        }

        bool operator!=(const Vector2& Other) const
        {
            return this->x != Other.x ||
                this->y != Other.y;
        }

        bool operator!=(float Other) const
        {
            return *this == Vector2(Other);
        }

        template<typename T>
        bool operator>=(const T& Other) const
        {
            return *this >= Vector2(Other.x, Other.y);
        }

        bool operator>=(const Vector2& Other) const
        {
            return this->x >= Other.x &&
                this->y >= Other.y;
        }

        bool operator>=(float Other) const
        {
            return *this >= Vector2(Other);
        }

        template<typename T>
        bool operator<=(const T& Other) const
        {
            return *this <= Vector2(Other.x, Other.y);
        }

        bool operator<=(const Vector2& Other) const
        {
            return this->x <= Other.x &&
                this->y <= Other.y;
        }

        bool operator<=(float Other) const
        {
            return *this <= Vector2(Other);
        }

        template<typename T>
        bool operator>(const T& Other) const
        {
            return *this > Vector2(Other.x, Other.y);
        }

        bool operator>(const Vector2& Other) const
        {
            return this->x > Other.x &&
                this->y > Other.y;
        }

        bool operator>(float Other) const
        {
            return *this > Vector2(Other);
        }

        template<typename T>
        bool operator<(const T& Other) const
        {
            return *this < Vector2(Other.x, Other.y);
        }

        bool operator<(const Vector2& Other) const
        {
            return this->x < Other.x&&
                this->y < Other.y;
        }

        bool operator<(float Other) const
        {
            return *this < Vector2(Other);
        }

        template<typename T>
        Vector2 operator+(const T& Other) const
        {
            return *this + Vector2(Other.x, Other.y);
        }

        Vector2 operator+(const Vector2& Other) const
        {
            return Vector2(
                this->x + Other.x,
                this->y + Other.y
            );
        }

        Vector2 operator+(float Other) const
        {
            return *this + Vector2(Other);
        }

        template<typename T>
        Vector2 operator-(const T& Other) const
        {
            return *this - Vector2(Other.x, Other.y);
        }

        Vector2 operator-(const Vector2& Other) const
        {
            return Vector2(
                this->x - Other.x,
                this->y - Other.y
            );
        }

        Vector2 operator-(float Other) const
        {
            return *this - Vector2(Other);
        }

        template<typename T>
        Vector2 operator*(const T& Other) const
        {
            return *this * Vector2(Other.x, Other.y);
        }

        Vector2 operator*(const Vector2& Other) const
        {
            return Vector2(
                this->x * Other.x,
                this->y * Other.y
            );
        }

        Vector2 operator*(float Other) const
        {
            return *this * Vector2(Other);
        }

        template<typename T>
        Vector2 operator/(const T& Other) const
        {
            return *this / Vector2(Other.x, Other.y);
        }

        Vector2 operator/(const Vector2& Other) const
        {
            return Vector2(
                this->x / Other.x,
                this->y / Other.y
            );
        }

        Vector2 operator/(float Other) const
        {
            return *this / Vector2(Other);
        }
    };
	
    class Engine_API Vector3 : public Vector2 {
    public:
        union { float z, b, p; };

        Vector3() : Vector2(0.f), z(0.f) {}

        template <typename T>
        Vector3(const T& Val) : Vector2(static_cast<float>(Val)), z(static_cast<float>(Val)) {}

        template <typename T>
        Vector3(const T& xVal, const T& yVal) : Vector2(xVal, yVal), z(0.f) {}

        template <typename T>
        Vector3(const T& xVal, const T& yVal, const T& zVal) : Vector2(xVal, yVal), z(static_cast<float>(zVal)) {}

        Vector3(const glm::vec3& Vector) : Vector2(Vector.x, Vector.y), z(Vector.z) {}
        Vector3(const b2Vec3& Vector) : Vector2(Vector.x, Vector.y), z(Vector.z) {}

        operator glm::vec3() const { return glm::vec3(x, y, z); }
        operator b2Vec3() const { return b2Vec3(x, y, z); }

        template<typename T>
        Vector3& operator=(const T& Other)
        {
            *this = Vector3(Other.x, Other.y, Other.z);
            return *this;
        }

        Vector3& operator=(float Other)
        {
            *this = Vector3(Other);
            return *this;
        }

        template<typename T>
        Vector3& operator+=(const T& Other)
        {
            *this += Vector3(Other.x, Other.y, Other.z);
            return *this;
        }

        Vector3& operator+=(const Vector3& Other)
        {
            this->x += Other.x;
            this->y += Other.y;
            this->z += Other.z;
            return *this;
        }

        Vector3& operator+=(float Other)
        {
            *this += Vector3(Other);
            return *this;
        }

        template<typename T>
        Vector3& operator-=(const T& Other)
        {
            *this -= Vector3(Other.x, Other.y, Other.z);
            return *this;
        }

        Vector3& operator-=(const Vector3& Other)
        {
            this->x -= Other.x;
            this->y -= Other.y;
            this->z -= Other.z;
            return *this;
        }

        Vector3& operator-=(float Other)
        {
            *this -= Vector3(Other);
            return *this;
        }

        template<typename T>
        Vector3& operator*=(const T& Other)
        {
            *this *= Vector3(Other.x, Other.y, Other.z);
            return *this;
        }

        Vector3& operator*=(const Vector3& Other)
        {
            this->x *= Other.x;
            this->y *= Other.y;
            this->z *= Other.z;
            return *this;
        }

        Vector3& operator*=(float Other)
        {
            *this *= Vector3(Other);
            return *this;
        }

        template<typename T>
        Vector3& operator/=(const T& Other)
        {
            *this /= Vector3(Other.x, Other.y, Other.z);
            return *this;
        }

        Vector3& operator/=(const Vector3& Other)
        {
            this->x /= Other.x;
            this->y /= Other.y;
            this->z /= Other.z;

            return *this;
        }

        Vector3& operator/=(float Other)
        {
            *this /= Vector3(Other);
            return *this;
        }

        template<typename T>
        bool operator==(const T& Other) const
        {
            return *this == Vector3(Other.x, Other.y, Other.z);
        }

        bool operator==(const Vector3& Other) const
        {
            return this->x == Other.x &&
                this->y == Other.y &&
                this->z == Other.z;
        }

        bool operator==(float Other) const
        {
            return *this == Vector3(Other);
        }

        template<typename T>
        bool operator!=(const T& Other) const
        {
            return *this == Vector3(Other.x, Other.y, Other.z);
        }

        bool operator!=(const Vector3& Other) const
        {
            return this->x != Other.x ||
                this->y != Other.y ||
                this->z != Other.z;
        }

        bool operator!=(float Other) const
        {
            return *this == Vector3(Other);
        }

        template<typename T>
        bool operator>=(const T& Other) const
        {
            return *this >= Vector3(Other.x, Other.y, Other.z);
        }

        bool operator>=(const Vector3& Other) const
        {
            return this->x >= Other.x &&
                this->y >= Other.y &&
                this->z > Other.z;
        }

        bool operator>=(float Other) const
        {
            return *this >= Vector3(Other);
        }

        template<typename T>
        bool operator<=(const T& Other) const
        {
            return *this <= Vector3(Other.x, Other.y, Other.z);
        }

        bool operator<=(const Vector3& Other) const
        {
            return this->x <= Other.x &&
                this->y <= Other.y &&
                this->z > Other.z;
        }

        bool operator<=(float Other) const
        {
            return *this <= Vector3(Other);
        }

        template<typename T>
        bool operator>(const T& Other) const
        {
            return *this > Vector3(Other.x, Other.y, Other.y);
        }

        bool operator>(const Vector3& Other) const
        {
            return this->x > Other.x &&
                this->y > Other.y &&
                this->z > Other.z;
        }

        bool operator>(float Other) const
        {
            return *this > Vector3(Other);
        }

        template<typename T>
        bool operator<(const T& Other) const
        {
            return *this < Vector3(Other.x, Other.y, Other.z);
        }

        bool operator<(const Vector3& Other) const
        {
            return this->x < Other.x&&
                this->y < Other.y&&
                this->z < Other.z;
        }

        bool operator<(float Other) const
        {
            return *this < Vector3(Other);
        }

        template<typename T>
        Vector3 operator+(const T& Other) const
        {
            return *this + Vector3(Other.x, Other.y, Other.z);
        }

        Vector3 operator+(const Vector3& Other) const
        {
            return Vector3(
                this->x + Other.x,
                this->y + Other.y,
                this->z + Other.z
            );
        }

        Vector3 operator+(float Other) const
        {
            return *this + Vector3(Other);
        }

        template<typename T>
        Vector3 operator-(const T& Other) const
        {
            return *this - Vector3(Other.x, Other.y, Other.z);
        }

        Vector3 operator-(const Vector3& Other) const
        {
            return Vector3(
                this->x - Other.x,
                this->y - Other.y,
                this->z - Other.z
            );
        }

        Vector3 operator-(float Other) const
        {
            return *this - Vector3(Other);
        }

        template<typename T>
        Vector3 operator*(const T& Other) const
        {
            return *this * Vector3(Other.x, Other.y, Other.z);
        }

        Vector3 operator*(const Vector3& Other) const
        {
            return Vector3(
                this->x * Other.x,
                this->y * Other.y,
                this->z * Other.z
            );
        }

        Vector3 operator*(float Other) const
        {
            return *this * Vector3(Other);
        }

        template<typename T>
        Vector3 operator/(const T& Other) const
        {
            return *this / Vector3(Other.x, Other.y, Other.z);
        }

        Vector3 operator/(const Vector3& Other) const
        {
            return Vector3(
                this->x / Other.x,
                this->y / Other.y,
                this->z / Other.z
            );
        }

        Vector3 operator/(float Other) const
        {
            return *this / Vector3(Other);
        }
    };
    
    class Engine_API Vector4 : public Vector3 {
    public:
        union { float w, a, q; };

        Vector4() : Vector3(0.f), w(0.f) {}

        template <typename T>
        Vector4(const T& Val) : Vector3(static_cast<float>(Val)), w(static_cast<float>(Val)) {}

        template <typename T>
        Vector4(const T& xVal, const T& yVal) : Vector3(xVal, yVal), w(1.f) {}

        template <typename T>
        Vector4(const T& xVal, const T& yVal, const T& zVal) : Vector3(xVal, yVal, zVal), w(1.f) {}

        template <typename T>
        Vector4(const T& xVal, const T& yVal, const T& zVal, const T& wVal) : Vector3(xVal, yVal, zVal), w(static_cast<float>(wVal)) {}
    	
        Vector4(const glm::vec4& Vector) : Vector3(Vector.x, Vector.y, Vector.z), w(Vector.w) {}
        Vector4(const SkColor& Vector) : Vector3(
            SkColorGetR(Vector) / 255.f,
            SkColorGetG(Vector) / 255.f,
            SkColorGetB(Vector) / 255.f),
    		a(SkColorGetA(Vector) / 255.f) {}
        Vector4(const b2Vec3& Vector) : Vector3(Vector.x, Vector.y, Vector.z), w(1.f) {}

        operator glm::vec4() const { return glm::vec4(x, y, z, w); }
        operator SkColor() const { return SkColorSetARGB(
            static_cast<uint32_t>(a * 255.f),
            static_cast<uint32_t>(r * 255.f),
            static_cast<uint32_t>(g * 255.f),
            static_cast<uint32_t>(b * 255.f)
        ); }
        operator b2Vec3() const { return b2Vec3(x, y, z); }

        template<typename T>
        Vector4& operator=(const T& Other)
        {
            *this = Vector4(Other.x, Other.y, Other.z, Other.w);
            return *this;
        }

        Vector4& operator=(float Other)
        {
            *this = Vector4(Other);
            return *this;
        }

        template<typename T>
        Vector4& operator+=(const T& Other)
        {
            *this += Vector4(Other.x, Other.y, Other.z, Other.w);
            return *this;
        }

        Vector4& operator+=(const Vector4& Other)
        {
            this->x += Other.x;
            this->y += Other.y;
            this->z += Other.z;
            this->w += Other.w;
            return *this;
        }

        Vector4& operator+=(float Other)
        {
            *this += Vector4(Other);
            return *this;
        }

        template<typename T>
        Vector4& operator-=(const T& Other)
        {
            *this -= Vector4(Other.x, Other.y, Other.z, Other.w);
            return *this;
        }

        Vector4& operator-=(const Vector4& Other)
        {
            this->x -= Other.x;
            this->y -= Other.y;
            this->z -= Other.z;
            this->w -= Other.w;
            return *this;
        }

        Vector4& operator-=(float Other)
        {
            *this -= Vector4(Other);
            return *this;
        }

        template<typename T>
        Vector4& operator*=(const T& Other)
        {
            *this *= Vector4(Other.x, Other.y, Other.z, Other.w);
            return *this;
        }

        Vector4& operator*=(const Vector4& Other)
        {
            this->x *= Other.x;
            this->y *= Other.y;
            this->z *= Other.z;
            this->w *= Other.w;
            return *this;
        }

        Vector4& operator*=(float Other)
        {
            *this *= Vector4(Other);
            return *this;
        }

        template<typename T>
        Vector4& operator/=(const T& Other)
        {
            *this /= Vector4(Other.x, Other.y, Other.z, Other.w);
            return *this;
        }

        Vector4& operator/=(const Vector4& Other)
        {
            this->x /= Other.x;
            this->y /= Other.y;
            this->z /= Other.z;
            this->w /= Other.w;

            return *this;
        }

        Vector4& operator/=(float Other)
        {
            *this /= Vector4(Other);
            return *this;
        }

        template<typename T>
        bool operator==(const T& Other) const
        {
            return *this == Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        bool operator==(const Vector4& Other) const
        {
            return this->x == Other.x &&
                this->y == Other.y &&
                this->z == Other.z &&
                this->w == Other.w;
        }

        bool operator==(float Other) const
        {
            return *this == Vector4(Other);
        }

        template<typename T>
        bool operator!=(const T& Other) const
        {
            return *this == Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        bool operator!=(const Vector4& Other) const
        {
            return this->x != Other.x ||
                this->y != Other.y ||
                this->z != Other.z ||
                this->w != Other.w;
        }

        bool operator!=(float Other) const
        {
            return *this == Vector4(Other);
        }

        template<typename T>
        bool operator>=(const T& Other) const
        {
            return *this >= Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        bool operator>=(const Vector4& Other) const
        {
            return this->x >= Other.x &&
                this->y >= Other.y &&
                this->z >= Other.z &&
                this->w >= Other.w;
        }

        bool operator>=(float Other) const
        {
            return *this >= Vector4(Other);
        }

        template<typename T>
        bool operator<=(const T& Other) const
        {
            return *this <= Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        bool operator<=(const Vector4& Other) const
        {
            return this->x <= Other.x &&
                this->y <= Other.y &&
                this->z <= Other.z &&
                this->w <= Other.w;
        }

        bool operator<=(float Other) const
        {
            return *this <= Vector4(Other);
        }

        template<typename T>
        bool operator>(const T& Other) const
        {
            return *this > Vector4(Other.x, Other.y, Other.y, Other.w);
        }

        bool operator>(const Vector4& Other) const
        {
            return this->x > Other.x &&
                this->y > Other.y &&
                this->z > Other.z &&
                this->w > Other.w;
        }

        bool operator>(float Other) const
        {
            return *this > Vector4(Other);
        }

        template<typename T>
        bool operator<(const T& Other) const
        {
            return *this < Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        bool operator<(const Vector4& Other) const
        {
            return this->x < Other.x &&
                this->y < Other.y &&
                this->z < Other.z &&
                this->w < Other.w;
        }

        bool operator<(float Other) const
        {
            return *this < Vector4(Other);
        }

        template<typename T>
        Vector4 operator+(const T& Other) const
        {
            return *this + Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        Vector4 operator+(const Vector4& Other) const
        {
            return Vector4(
                this->x + Other.x,
                this->y + Other.y,
                this->z + Other.z,
                this->w + Other.w
            );
        }

        Vector4 operator+(float Other) const
        {
            return *this + Vector4(Other);
        }

        template<typename T>
        Vector4 operator-(const T& Other) const
        {
            return *this - Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        Vector4 operator-(const Vector4& Other) const
        {
            return Vector4(
                this->x - Other.x,
                this->y - Other.y,
                this->z - Other.z,
                this->w - Other.w
            );
        }

        Vector4 operator-(float Other) const
        {
            return *this - Vector4(Other);
        }

        template<typename T>
        Vector4 operator*(const T& Other) const
        {
            return *this * Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        Vector4 operator*(const Vector4& Other) const
        {
            return Vector4(
                this->x * Other.x,
                this->y * Other.y,
                this->z * Other.z,
                this->w * Other.w
            );
        }

        Vector4 operator*(float Other) const
        {
            return *this * Vector4(Other);
        }

        template<typename T>
        Vector4 operator/(const T& Other) const
        {
            return *this / Vector4(Other.x, Other.y, Other.z, Other.w);
        }

        Vector4 operator/(const Vector4& Other) const
        {
            return Vector4(
                this->x / Other.x,
                this->y / Other.y,
                this->z / Other.z,
                this->w / Other.w
            );
        }

        Vector4 operator/(float Other) const
        {
            return *this / Vector4(Other);
        }
    };

    typedef Vector4 Color;

    typedef glm::ivec1 IVector1;
    typedef glm::ivec2 IVector2;
    typedef glm::ivec3 IVector3;
    typedef glm::ivec4 IVector4;

    typedef glm::uvec1 UVector1;
    typedef glm::uvec2 UVector2;
    typedef glm::uvec3 UVector3;
    typedef glm::uvec4 UVector4;

    typedef glm::dvec1 DVector1;
    typedef glm::dvec2 DVector2;
    typedef glm::dvec3 DVector3;
    typedef glm::dvec4 DVector4;

    typedef glm::mat4 Matrix;
    typedef glm::quat Quaternion;
}