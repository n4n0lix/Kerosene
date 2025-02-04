#pragma once

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Includes                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// Std-Includes
#include <iostream>
#include <exception>

// Other Includes

// Internal Includes
#include "_gl.h"
#include "_global.h"
#include "glwindow.h"
#include "inputengine.h"

// Testing Includes (temporary)
#include "primitivetype.h"
#include "camera3d.h"
#include "camera2d.h"

#include "texture.h"
#include "shader.h"
#include "material.h"
#include "renderresource.h"

#include "vertex_pc.h"
#include "vertex_pt.h"
#include "textureslot.h"

#include "imageutils.h"
#include "stringutils.h"

#include "glbuffer.h"

#include "scene.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Class                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ENGINE_NAMESPACE_BEGIN

class Entity;
class Scene;
class RenderResource;

class RenderEngine : public enable_weak_from_this<RenderEngine>
{
public:

    const int32_t GL_VERSION_MAJOR = 3;
    const int32_t GL_VERSION_MINOR = 3;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Public                          */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            RenderEngine() {}
            ~RenderEngine() {}

    // GENERAL
    void on_start( weak<InputEngine> input );
    void on_render( float extrapolation );
    void on_shutdown();
    void unload_everything();
    void on_gamestate_end();

    // UTILITY
    bool                is_exit_requested();
    void                hide_cursor( bool hideCursor );
    weak<GLWindow>      get_window();
    owner<Texture>      load_texture( string filename, TextureOptions options = TextureOptions() );

    // RESOURCES
    weak<Texture>       add_texture( string filename, owner<Texture> texture );
    weak<Texture>       get_texture( string filename );
    bool                has_texture( string filename );

    weak<Shader>        add_shader( string filename, owner<Shader> shader );
    weak<Shader>        get_shader( string filename );
    bool                has_shader( string filename );

    template<typename T>
    weak<T> add_resource( string resName, owner<T> res );

    weak<Scene>         add_scene();
    owner<Scene>        remove_scene( weak<Scene> scene );

private:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Private                         */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    void init_context_and_window();
    void setup_builtin_shaders();
    void destroy_context_and_window();

    owner<GLWindow> _mainWindow;

    std::vector<owner<Scene>>            _scenes;
    std::map< string, owner<Texture> >	 _textures;
    std::map< string, owner<Shader> >	 _shaders;

    std::vector< weak<RenderResource> >       _uninitializedResources;
    std::map< string, owner<RenderResource> > _resources;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                     Private Static                     */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    static Logger LOGGER;

};

template<typename T>
weak<T> RenderEngine::add_resource( string resName, owner<T> res )
{
    static_assert(std::is_base_of<RenderResource, T>::value, "T must inherit from RenderResource");

    auto weakR = (weak<RenderResource>) res.get_non_owner();
    auto weakT = res.get_non_owner();

    _uninitializedResources.push_back( weakR );
    _resources.emplace( std::make_pair( resName, std::move( res ) ) );

    return weakT;
}

ENGINE_NAMESPACE_END