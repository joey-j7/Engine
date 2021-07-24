#pragma once

#include "box2d/b2_collision.h"

namespace Engine
{
    typedef b2AABB AABB;

    class Engine_API CollisionCheck
    {
    public:
        static bool Contains(const AABB& AABB, const Vector2& Vec)
        {
            return Vec >= AABB.lowerBound && Vec <= AABB.upperBound;
        }
    };
}