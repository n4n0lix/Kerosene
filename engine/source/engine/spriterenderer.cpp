// Header
#include "spriterenderer.h"

ENGINE_NAMESPACE_BEGIN

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Public                         */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
SpriteRenderer::SpriteRenderer() : Renderer()
{

}

void SpriteRenderer::set_entity( weak<Entity> entity )
{
    _entity = entity;
}

void SpriteRenderer::on_init( RenderEngine& engine )
{
    _shader    = engine.get_shader( "builtin_texture" );
    _texture   = engine.get_texture( "res/textures/dev/128.png" );
    _material  = engine.add_material( "sprite_test_material", make_owner<Material>( _shader, _texture ) );

    _vao = make_owner<VertexArray<Vertex_pt>>();
    
    vector<Vertex_pt> vertices = vector<Vertex_pt>();
    vertices.push_back( Vertex_pt( Vector3f( 1, 0, -.5 ), Vector2f( 1.0f, 1.0f ) ) );
    vertices.push_back( Vertex_pt( Vector3f( 0, 0, -.5 ), Vector2f( 0.0f, 1.0f ) ) );
    vertices.push_back( Vertex_pt( Vector3f( 1, 1, -.5 ), Vector2f( 1.0f, 0.0f ) ) );
    vertices.push_back( Vertex_pt( Vector3f( 0, 0, -.5 ), Vector2f( 0.0f, 1.0f ) ) );
    vertices.push_back( Vertex_pt( Vector3f( 1, 1, -.5 ), Vector2f( 1.0f, 0.0f ) ) );
    vertices.push_back( Vertex_pt( Vector3f( 0, 1, -.5 ), Vector2f( 0.0f, 0.0f ) ) );

    _token = _vao->add_vertices( std::move( vertices ) );
    _vao->add_render_static( _token.get_non_owner() );
}

void SpriteRenderer::on_render( RenderEngine& engine, Camera& cam, Matrix4f& proj_view, float amount )
{
    Transform curTransform = _entity->transform;
    Transform lastTransform = _lastTransform;
    Transform transform;
    if ( extrapolation ) {
        transform = Transform::lerp( lastTransform, curTransform, amount );
    }
    else {
        transform = _entity->transform;
    }


    Matrix4f wvp = proj_view * transform.to_mat4f();
    _material->get_shader()->set_vertex_uniform( Uniform::WORLD_VIEW_PROJ_MATRIX, wvp );
    _material->bind();
    _vao->render();

    _lastTransform = curTransform;
}

void SpriteRenderer::on_cleanup( RenderEngine& engine )
{
    _vao.destroy();
    _token.destroy();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Private                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ENGINE_NAMESPACE_END
