#pragma once

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Includes                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// Std-Includes

// Other Includes

// Internal Includes
#include "_global.h"
#include "camera.h"
#include "entity.h"
#include "batch.h"
#include "renderer.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Class                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ENGINE_NAMESPACE_BEGIN

class Renderer;
class RenderEngine;

class Scene
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Public                          */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
public:
                Scene() = default;
                ~Scene() = default;


    weak<Camera>              add_camera( owner<Camera> );
    owner<Camera>&&           remove_camera( weak<Camera> );
    
    weak<Renderer>            add_renderer( owner<Renderer> );
    owner<Renderer>&&         remove_renderer( weak<Renderer> );

    void                      render( RenderEngine&, float extrapolation );
    void                      cleanup( RenderEngine& );

protected:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                       Protected                        */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

private:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Private                         */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    vector<owner<Camera>>     _cameras;

    vector<owner<Renderer>>  _ownerRenderers;
    vector<weak<Renderer>>   _uninitRenderers;
    vector<weak<Renderer>>   _renderers;

};

ENGINE_NAMESPACE_END
