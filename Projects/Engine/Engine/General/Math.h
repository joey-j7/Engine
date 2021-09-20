#pragma once

#include "Engine/Core.h"

#include <glm/glm.hpp>
#include <include/core/SkColor.h>

#include "box2d/b2_math.h"

namespace Engine
{
	template <typename F = float>
    class Engine_API XVector2 {
    public:
        union { F x, r, s; };
        union { F y, g, t; };

        XVector2() : x(0.f), y(0.f) {}

        template <typename T>
        XVector2(const T& Val) : x(static_cast<F>(Val)), y(static_cast<F>(Val)) {}

        template <typename T>
        XVector2(const T& xVal, const T& yVal) : x(static_cast<F>(xVal)), y(static_cast<F>(yVal)) {}

        template <typename T>
        XVector2(const XVector2<T>& Vector) : x(Vector.x), y(Vector.y) {}
        XVector2(const glm::vec2& Vector) : x(Vector.x), y(Vector.y) {}
        XVector2(const b2Vec2& Vector) : x(Vector.x), y(Vector.y) {}
        XVector2(const SkVector& Vector) : x(Vector.fX), y(Vector.fY) {}

        template <typename T>
        operator XVector2<T>() const { return XVector2<T>(x, y); }
        operator glm::vec2() const { return glm::vec2(x, y); }
        operator b2Vec2() const { return b2Vec2(x, y); }
        operator SkVector() const { return SkVector::Make(x, y); }
        
        template<typename T>
        XVector2<F>& operator=(const T& Other)
        {
            *this = XVector2<F>(Other.x, Other.y);
            return *this;
        }

        XVector2<F>& operator=(F Other)
        {
            *this = XVector2<F>(Other);
            return *this;
        }

        template<typename T>
        XVector2<F>& operator+=(const T& Other)
        {
            *this += XVector2<F>(Other);
            return *this;
        }

        XVector2<F>& operator+=(const XVector2<F>& Other)
        {
            this->x += Other.x;
            this->y += Other.y;
            return *this;
        }
        
        template<typename T>
        XVector2<F>& operator-=(const T& Other)
        {
            *this -= XVector2<F>(Other);
            return *this;
        }

        XVector2<F>& operator-=(const XVector2<F>& Other)
        {
            this->x -= Other.x;
            this->y -= Other.y;
            return *this;
        }

        template<typename T>
        XVector2<F>& operator*=(const T& Other)
        {
            *this *= XVector2<F>(Other);
            return *this;
        }

        XVector2<F>& operator*=(const XVector2<F>& Other)
        {
            this->x *= Other.x;
            this->y *= Other.y;
            return *this;
        }
        
        template<typename T>
        XVector2<F>& operator/=(const T& Other)
        {
            *this /= XVector2<F>(Other);
            return *this;
        }

        XVector2<F>& operator/=(const XVector2<F>& Other)
        {
            this->x /= Other.x;
            this->y /= Other.y;
            return *this;
        }
        
        template<typename T>
        bool operator==(const T& Other) const
        {
            return *this == XVector2<F>(Other);
        }

        bool operator==(const XVector2<F>& Other) const
        {
            return this->x == Other.x &&
                this->y == Other.y;
        }

        template<typename T>
        bool operator!=(const T& Other) const
        {
            return *this == XVector2<F>(Other);
        }

        bool operator!=(const XVector2<F>& Other) const
        {
            return this->x != Other.x ||
                this->y != Other.y;
        }

        template<typename T>
        bool operator>=(const T& Other) const
        {
            return *this >= XVector2<F>(Other);
        }

        bool operator>=(const XVector2<F>& Other) const
        {
            return this->x >= Other.x &&
                this->y >= Other.y;
        }
        
        template<typename T>
        bool operator<=(const T& Other) const
        {
            return *this <= XVector2<F>(Other);
        }

        bool operator<=(const XVector2<F>& Other) const
        {
            return this->x <= Other.x &&
                this->y <= Other.y;
        }
        
        template<typename T>
        bool operator>(const T& Other) const
        {
            return *this > XVector2<F>(Other);
        }

        bool operator>(const XVector2<F>& Other) const
        {
            return this->x > Other.x &&
                this->y > Other.y;
        }
        
        template<typename T>
        bool operator<(const T& Other) const
        {
            return *this < XVector2<F>(Other);
        }

        bool operator<(const XVector2<F>& Other) const
        {
            return this->x < Other.x&&
                this->y < Other.y;
        }

        template<typename T>
        XVector2<F> operator+(const T& Other) const
        {
            return *this + XVector2<F>(Other);
        }

        XVector2<F> operator+(const XVector2<F>& Other) const
        {
            return XVector2(
                this->x + Other.x,
                this->y + Other.y
            );
        }
        
        template<typename T>
        XVector2<F> operator-(const T& Other) const
        {
            return *this - XVector2<F>(Other);
        }

        XVector2<F> operator-(const XVector2<F>& Other) const
        {
            return XVector2(
                this->x - Other.x,
                this->y - Other.y
            );
        }

        template<typename T>
        XVector2<F> operator*(const T& Other) const
        {
            return *this * XVector2<F>(Other);
        }
        
        XVector2<F> operator*(const XVector2<F>& Other) const
        {
            return XVector2(
                this->x * Other.x,
                this->y * Other.y
            );
        }
        
        template<typename T>
        XVector2<F> operator/(const T& Other) const
        {
            return *this / XVector2<F>(Other);
        }

        XVector2<F> operator/(const XVector2<F>& Other) const
        {
            return XVector2(
                this->x / Other.x,
                this->y / Other.y
            );
        }

        virtual F Length() const
        {
            return glm::sqrt(this->x * this->x + this->y * this->y);
        }
    };

	template <typename F = float>
    class Engine_API XVector3 : public XVector2<F> {
    public:
        union { F z, b, p; };

        XVector3() : XVector2<F>(0.f), z(0.f) {}

        template <typename T>
        XVector3(const T& Val) : XVector2<F>(static_cast<F>(Val)), z(static_cast<F>(Val)) {}

        template <typename T>
        XVector3(const T& xVal, const T& yVal) : XVector2<F>(xVal, yVal), z(0.f) {}

        template <typename T>
        XVector3(const T& xVal, const T& yVal, const T& zVal) : XVector2<F>(xVal, yVal), z(static_cast<F>(zVal)) {}

        template <typename T>
        XVector3(const XVector2<T>& Vector) : XVector2<F>(Vector.x, Vector.y), z(0.f) {}

        template <typename T>
        XVector3(const XVector3<T>& Vector) : XVector2<F>(Vector.x, Vector.y), z(Vector.z) {}
    	
        XVector3(const glm::vec3& Vector) : XVector2<F>(Vector.x, Vector.y), z(Vector.z) {}
        XVector3(const b2Vec3& Vector) : XVector2<F>(Vector.x, Vector.y), z(Vector.z) {}

        template <typename T>
        operator XVector3<T>() const { return XVector3<T>(this->x, this->y, z); }
        operator glm::vec3() const { return glm::vec3(this->x, this->y, z); }
        operator b2Vec3() const { return b2Vec3(this->x, this->y, z); }

        template<typename T>
        XVector3<F>& operator=(const T& Other)
        {
            *this = XVector3<F>(Other);
            return *this;
        }

        XVector3<F>& operator=(F Other)
        {
            *this = XVector3<F>(Other);
            return *this;
        }

        template<typename T>
        XVector3<F>& operator+=(const T& Other)
        {
            *this += XVector3<F>(Other);
            return *this;
        }

        XVector3<F>& operator+=(const XVector3<F>& Other)
        {
            this->x += Other.x;
            this->y += Other.y;
            this->z += Other.z;
            return *this;
        }

        template<typename T>
        XVector3<F>& operator-=(const T& Other)
        {
            *this -= XVector3<F>(Other.x);
            return *this;
        }

        XVector3<F>& operator-=(const XVector3<F>& Other)
        {
            this->x -= Other.x;
            this->y -= Other.y;
            this->z -= Other.z;
            return *this;
        }
        
        template<typename T>
        XVector3<F>& operator*=(const T& Other)
        {
            *this *= XVector3<F>(Other);
            return *this;
        }

        XVector3<F>& operator*=(const XVector3<F>& Other)
        {
            this->x *= Other.x;
            this->y *= Other.y;
            this->z *= Other.z;
            return *this;
        }

        template<typename T>
        XVector3<F>& operator/=(const T& Other)
        {
            *this /= XVector3<F>(Other);
            return *this;
        }

        XVector3<F>& operator/=(const XVector3<F>& Other)
        {
            this->x /= Other.x;
            this->y /= Other.y;
            this->z /= Other.z;

            return *this;
        }
        
        template<typename T>
        bool operator==(const T& Other) const
        {
            return *this == XVector3<F>(Other);
        }

        bool operator==(const XVector3<F>& Other) const
        {
            return this->x == Other.x &&
                this->y == Other.y &&
                this->z == Other.z;
        }
        
        template<typename T>
        bool operator!=(const T& Other) const
        {
            return *this == XVector3<F>(Other);
        }

        bool operator!=(const XVector3<F>& Other) const
        {
            return this->x != Other.x ||
                this->y != Other.y ||
                this->z != Other.z;
        }
        
        template<typename T>
        bool operator>=(const T& Other) const
        {
            return *this >= XVector3<F>(Other);
        }

        bool operator>=(const XVector3<F>& Other) const
        {
            return this->x >= Other.x &&
                this->y >= Other.y &&
                this->z > Other.z;
        }
        
        template<typename T>
        bool operator<=(const T& Other) const
        {
            return *this <= XVector3<F>(Other);
        }

        bool operator<=(const XVector3<F>& Other) const
        {
            return this->x <= Other.x &&
                this->y <= Other.y &&
                this->z > Other.z;
        }
        
        template<typename T>
        bool operator>(const T& Other) const
        {
            return *this > XVector3<F>(Other.x);
        }

        bool operator>(const XVector3<F>& Other) const
        {
            return this->x > Other.x &&
                this->y > Other.y &&
                this->z > Other.z;
        }
        
        template<typename T>
        bool operator<(const T& Other) const
        {
            return *this < XVector3<F>(Other);
        }

        bool operator<(const XVector3<F>& Other) const
        {
            return this->x < Other.x&&
                this->y < Other.y&&
                this->z < Other.z;
        }
        
        template<typename T>
        XVector3<F> operator+(const T& Other) const
        {
            return *this + XVector3<F>(Other);
        }

        XVector3<F> operator+(const XVector3<F>& Other) const
        {
            return XVector3<F>(
                this->x + Other.x,
                this->y + Other.y,
                this->z + Other.z
            );
        }
        
        template<typename T>
        XVector3<F> operator-(const T& Other) const
        {
            return *this - XVector3<F>(Other);
        }

        XVector3<F> operator-(const XVector3<F>& Other) const
        {
            return XVector3<F>(
                this->x - Other.x,
                this->y - Other.y,
                this->z - Other.z
            );
        }
    	
        template<typename T>
        XVector3<F> operator*(const T& Other) const
        {
            return *this * XVector3<F>(Other);
        }

        XVector3<F> operator*(const XVector3<F>& Other) const
        {
            return XVector3<F>(
                this->x * Other.x,
                this->y * Other.y,
                this->z * Other.z
            );
        }
        
        template<typename T>
        XVector3<F> operator/(const T& Other) const
        {
            return *this / XVector3<F>(Other);
        }

        XVector3<F> operator/(const XVector3<F>& Other) const
        {
            return XVector3<F>(
                this->x / Other.x,
                this->y / Other.y,
                this->z / Other.z
            );
        }

        virtual F Length() const override
        {
            return glm::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
        }
    };

    template <typename F = float>
    class Engine_API XVector4 : public XVector3<F> {
    public:
        union { F w, a, q; };

        XVector4() : XVector3<F>(0.f), w(0.f) {}

        template <typename T>
        XVector4(const T& Val) : XVector3<F>(static_cast<F>(Val)), w(static_cast<F>(Val)) {}

        template <typename T>
        XVector4(const T& xVal, const T& yVal) : XVector3<F>(xVal, yVal), w(1.f) {}

        template <typename T>
        XVector4(const T& xVal, const T& yVal, const T& zVal) : XVector3<F>(xVal, yVal, zVal), w(1.f) {}

        template <typename T>
        XVector4(const T& xVal, const T& yVal, const T& zVal, const T& wVal) : XVector3<F>(xVal, yVal, zVal), w(static_cast<F>(wVal)) {}

        template <typename T>
        XVector4(const XVector2<T>& Vector) : XVector3<F>(Vector.x, Vector.y, 0.f), w(0.f) {}

    	template <typename T>
        XVector4(const XVector3<T>& Vector) : XVector3<F>(Vector.x, Vector.y, Vector.z), w(0.f) {}
    	
        template <typename T>
        XVector4(const XVector4<T>& Vector) : XVector3<F>(Vector.x, Vector.y, Vector.z), w(Vector.w) {}
    	
        XVector4(const glm::vec4& Vector) : XVector3<F>(Vector.x, Vector.y, Vector.z), w(Vector.w) {}
        XVector4(const SkColor& Vector) : XVector3<F>(
            SkColorGetR(Vector) / 255.f,
            SkColorGetG(Vector) / 255.f,
            SkColorGetB(Vector) / 255.f),
    		a(SkColorGetA(Vector) / 255.f) {}
        XVector4(const b2Vec3& Vector) : XVector3<F>(Vector.x, Vector.y, Vector.z), w(1.f) {}

        template <typename T>
        operator XVector4<T>() const { return XVector4<T>(this->x, this->y, this->z, w); }
        operator glm::vec4() const { return glm::vec4(this->x, this->y, this->z, w); }
        operator SkColor() const { return SkColorSetARGB(
            static_cast<uint32_t>(this->a * 255.f),
            static_cast<uint32_t>(this->r * 255.f),
            static_cast<uint32_t>(this->g * 255.f),
            static_cast<uint32_t>(this->b * 255.f)
        ); }
        operator b2Vec3() const { return b2Vec3(this->x, this->y, this->z); }

        template<typename T>
        XVector4<F>& operator=(const T& Other)
        {
            *this = XVector4<F>(Other);
            return *this;
        }

        XVector4<F>& operator=(F Other)
        {
            *this = XVector4<F>(Other);
            return *this;
        }

        template<typename T>
        XVector4<F>& operator+=(const T& Other)
        {
            *this += XVector4<F>(Other);
            return *this;
        }

        XVector4<F>& operator+=(const XVector4<F>& Other)
        {
            this->x += Other.x;
            this->y += Other.y;
            this->z += Other.z;
            this->w += Other.w;
            return *this;
        }
        
        template<typename T>
        XVector4<F>& operator-=(const T& Other)
        {
            *this -= XVector4<F>(Other);
            return *this;
        }

        XVector4<F>& operator-=(const XVector4<F>& Other)
        {
            this->x -= Other.x;
            this->y -= Other.y;
            this->z -= Other.z;
            this->w -= Other.w;
            return *this;
        }
        
        template<typename T>
        XVector4<F>& operator*=(const T& Other)
        {
            *this *= XVector4<F>(Other);
            return *this;
        }

        XVector4<F>& operator*=(const XVector4<F>& Other)
        {
            this->x *= Other.x;
            this->y *= Other.y;
            this->z *= Other.z;
            this->w *= Other.w;
            return *this;
        }
        
        template<typename T>
        XVector4<F>& operator/=(const T& Other)
        {
            *this /= XVector4<F>(Other);
            return *this;
        }

        XVector4<F>& operator/=(const XVector4<F>& Other)
        {
            this->x /= Other.x;
            this->y /= Other.y;
            this->z /= Other.z;
            this->w /= Other.w;

            return *this;
        }

        template<typename T>
        bool operator==(const T& Other) const
        {
            return *this == XVector4<F>(Other);
        }

        bool operator==(const XVector4<F>& Other) const
        {
            return this->x == Other.x &&
                this->y == Other.y &&
                this->z == Other.z &&
                this->w == Other.w;
        }
        
        template<typename T>
        bool operator!=(const T& Other) const
        {
            return *this == XVector4<F>(Other);
        }

        bool operator!=(const XVector4<F>& Other) const
        {
            return this->x != Other.x ||
                this->y != Other.y ||
                this->z != Other.z ||
                this->w != Other.w;
        }
        
        template<typename T>
        bool operator>=(const T& Other) const
        {
            return *this >= XVector4<F>(Other);
        }

        bool operator>=(const XVector4<F>& Other) const
        {
            return this->x >= Other.x &&
                this->y >= Other.y &&
                this->z >= Other.z &&
                this->w >= Other.w;
        }
        
        template<typename T>
        bool operator<=(const T& Other) const
        {
            return *this <= XVector4<F>(Other);
        }

        bool operator<=(const XVector4<F>& Other) const
        {
            return this->x <= Other.x &&
                this->y <= Other.y &&
                this->z <= Other.z &&
                this->w <= Other.w;
        }

        template<typename T>
        bool operator>(const T& Other) const
        {
            return *this > XVector4<F>(Other);
        }

        bool operator>(const XVector4<F>& Other) const
        {
            return this->x > Other.x &&
                this->y > Other.y &&
                this->z > Other.z &&
                this->w > Other.w;
        }
        
        template<typename T>
        bool operator<(const T& Other) const
        {
            return *this < XVector4<F>(Other);
        }

        bool operator<(const XVector4<F>& Other) const
        {
            return this->x < Other.x &&
                this->y < Other.y &&
                this->z < Other.z &&
                this->w < Other.w;
        }
        
        template<typename T>
        XVector4<F> operator+(const T& Other) const
        {
            return *this + XVector3<F>(Other);
        }

        XVector4<F> operator+(const XVector4<F>& Other) const
        {
            return XVector4<F>(
                this->x + Other.x,
                this->y + Other.y,
                this->z + Other.z,
                this->w + Other.w
            );
        }

        template<typename T>
        XVector4<F> operator-(const T& Other) const
        {
            return *this - XVector3<F>(Other);
        }

        XVector4<F> operator-(const XVector4<F>& Other) const
        {
            return XVector4<F>(
                this->x - Other.x,
                this->y - Other.y,
                this->z - Other.z,
                this->w - Other.w
            );
        }
        
        template<typename T>
        XVector4<F> operator*(const T& Other) const
        {
            return *this * XVector3<F>(Other);
        }

        XVector4<F> operator*(const XVector4<F>& Other) const
        {
            return XVector4<F>(
                this->x * Other.x,
                this->y * Other.y,
                this->z * Other.z,
                this->w * Other.w
            );
        }
        
        template<typename T>
        XVector4<F> operator/(const T& Other) const
        {
            return *this / XVector3<F>(Other);
        }

        XVector4<F> operator/(const XVector4<F>& Other) const
        {
            return XVector4<F>(
                this->x / Other.x,
                this->y / Other.y,
                this->z / Other.z,
                this->w / Other.w
            );
        }

        virtual F Length() const override
        {
            return glm::sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
        }
    };

    typedef XVector2<float> Vector2;
    typedef XVector3<float> Vector3;
    typedef XVector4<float> Vector4;
    typedef Vector4 Color;
    
    typedef XVector2<int32_t> IVector2;
    typedef XVector3<int32_t> IVector3;
    typedef XVector4<int32_t> IVector4;
    
    typedef XVector2<uint32_t> UVector2;
    typedef XVector3<uint32_t> UVector3;
    typedef XVector4<uint32_t> UVector4;
    
    typedef XVector2<double> DVector2;
    typedef XVector3<double> DVector3;
    typedef XVector4<double> DVector4;

    typedef glm::mat4 Matrix;
    typedef glm::quat Quaternion;
}