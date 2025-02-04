#include "stdafx.h"

#include "engine.h"

#define ENGINE_DEBUG

#include "testgamestate.h"
#include "_gl.h"
#include "_global.h"

#include <bitset>
#include "owner.h"

using namespace ENGINE_NAMESPACE;

int main(int argc, char *argv[])
{
    #ifdef ENGINE_DEBUG
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

   Engine engine;
   return engine.run();
}
