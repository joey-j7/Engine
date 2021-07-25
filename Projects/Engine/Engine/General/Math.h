#pragma once

#include "Engine/Core.h"

#include <glm/glm.hpp>

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
            return *this >= Vector3(Other.x, Other.y);
        }

        bool operator>=(const Vector3& Other) const
        {
            return this->x >= Other.x &&
                this->y >= Other.y;
        }

        bool operator>=(float Other) const
        {
            return *this >= Vector3(Other);
        }

        template<typename T>
        bool operator<=(const T& Other) const
        {
            return *this <= Vector3(Other.x, Other.y);
        }

        bool operator<=(const Vector3& Other) const
        {
            return this->x <= Other.x &&
                this->y <= Other.y;
        }

        bool operator<=(float Other) const
        {
            return *this <= Vector3(Other);
        }

        template<typename T>
        bool operator>(const T& Other) const
        {
            return *this > Vector3(Other.x, Other.y);
        }

        bool operator>(const Vector3& Other) const
        {
            return this->x > Other.x &&
                this->y > Other.y;
        }

        bool operator>(float Other) const
        {
            return *this > Vector3(Other);
        }

        template<typename T>
        bool operator<(const T& Other) const
        {
            return *this < Vector3(Other.x, Other.y);
        }

        bool operator<(const Vector3& Other) const
        {
            return this->x < Other.x&&
                this->y < Other.y;
        }

        bool operator<(float Other) const
        {
            return *this < Vector3(Other);
        }
    };

    typedef glm::vec4 Vector4;
    typedef glm::vec4 Color;

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