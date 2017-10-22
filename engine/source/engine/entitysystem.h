#pragma once

// Std-Includes

// Other Includes

// Internal Includes
#include "_global.h"
#include "entity.h"

ENGINE_NAMESPACE_BEGIN

struct EntitySystem
{
    void update( Entity& transform );
    void create_snapshot_full( Entity& dest, const Entity& src );
};

ENGINE_NAMESPACE_END