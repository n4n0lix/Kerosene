// Header
#include "shader.h"
ENGINE_NAMESPACE_BEGIN

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                     Public Static                      */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Public                         */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Shader::bind(GLuint shaderId)
{
    if (CURRENT_SHADER != shaderId) {
        glUseProgram(shaderId);
        CURRENT_SHADER = shaderId;
    }
}

bool Shader::operator==(const Shader & o) const
{
    return _id == o._id;
}

bool Shader::operator!=(const Shader & o) const
{
    return !(*this == o);
}

bool Shader::operator<(const Shader & o1) const
{
    return _id < o1._id;
}

void Shader::bind()
{
    Shader::bind(_id);
}

Uniform Shader::vertex_uniform(string varname)
{
    return _vertexUniforms[varname];
}

Uniform Shader::frag_uniform(string varname )
{
    return _fragUniforms[varname];
}

Nullable<TextureSlot> Shader::frag_texture_slot(string name) 
{
    for (auto slot : _fragTextureSlots) {
        if (slot.name == name) {
            return Nullable<TextureSlot>(slot);
        }
    }

    return Nullable<TextureSlot>();
}

VertexLayout Shader::get_vertex_layout()
{
    return _vertexLayout;
}

void Shader::set_vertex_uniform( const Uniform& uniform, const Matrix4f& mat4 )
{
    set_uniform( uniform , mat4, _vertexUniforms );
}

void Shader::set_frag_uniform( const Uniform& uniform, const Matrix4f& mat4 )
{
    set_uniform( uniform, mat4, _fragUniforms );
}

void Shader::set_uniform( const Uniform& uniform, const Matrix4f& mat4, const map<string, Uniform>& uniforms ) {
    vector<float> matrix = mat4.column_major();
    uint32        location = uniforms.find(uniform.gl_varname())->second.gl_location();

    bind();
    glUniformMatrix4fv( location, 1, false, matrix.data() );
    // Transpose: false -> col-major / true -> row-major
}

void Shader::set_vertex_uniform( const Uniform& uniform, const Vector2f& vec2 )
{
    set_uniform( uniform, vec2, _vertexUniforms );
}

void Shader::set_frag_uniform( const Uniform& uniform, const Vector2f& vec2 )
{
    set_uniform( uniform, vec2, _fragUniforms );
}

void Shader::set_uniform( const Uniform& uniform, const Vector2f& vec2, const map<string, Uniform>& uniforms )
{
    uint32        location = uniforms.find( uniform.gl_varname() )->second.gl_location();

    bind();
    glUniform2f( location, vec2.x, vec2.y );
}

void Shader::set_vertex_uniform( const Uniform& uniform, const Vector3f& vec3 )
{
    set_uniform( uniform, vec3, _vertexUniforms );
}

void Shader::set_frag_uniform( const Uniform& uniform, const Vector3f& vec3 )
{
    set_uniform( uniform, vec3, _fragUniforms );
}

void Shader::set_uniform( const Uniform& uniform, const Vector3f& vec3, const map<string, Uniform>& uniforms )
{
    uint32        location = uniforms.find( uniform.gl_varname() )->second.gl_location();

    bind();
    glUniform3f( location, vec3.x, vec3.y, vec3.z );
}

void Shader::set_vertex_uniform( const Uniform& uniform, const Vector4f& vec4 )
{
    set_uniform( uniform, vec4, _vertexUniforms );
}

void Shader::set_frag_uniform( const Uniform& uniform, const Vector4f& vec4 )
{
    set_uniform( uniform, vec4, _fragUniforms );
}

void Shader::set_uniform( const Uniform& uniform, const Vector4f& vec4, const map<string, Uniform>& uniforms )
{
    uint32        location = uniforms.find( uniform.gl_varname() )->second.gl_location();

    bind();
    glUniform4f( location, vec4.x, vec4.y, vec4.z, vec4.w );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Private                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Shader::Shader( VertexLayout pLayout, vector<Uniform> pVUniforms, vector<Uniform> pFUniforms, vector<TextureSlot> pTexSlots, string pVertexCode, string pFragCode )
{
    _vertexLayout = pLayout;
    _fragTextureSlots = pTexSlots;

    GLuint vShaderId = create_vertex_shader( pLayout, pVUniforms, pVertexCode );
    GLuint fShaderId = create_frag_shader( pLayout, pVUniforms, pTexSlots, pFragCode );

    _id = link_shader( vShaderId, fShaderId );

    _vertexUniforms = process_uniforms( pVUniforms );
    _fragUniforms   = process_uniforms( pFUniforms );
    _fragTextureSlots = process_textureslots( pTexSlots );
}

Shader::~Shader()
{
    LOGGER.log(Level::DEBUG, _id) << "DELETE" << endl;
    glDeleteProgram( _id );
}

GLuint Shader::create_vertex_shader( VertexLayout pLayout, vector<Uniform> pVUniforms, string pVertexSrc )
{
    std::ostringstream vertexCode;

    // Header
    vertexCode
        << "// Generated by KeSh\n"
        << "#version 330 core\n"
        << "\n";

    // Uniforms
    for ( Uniform uniform : pVUniforms ) {
        vertexCode << UNIFORM( uniform );
    }

    // Vertex Components
    for ( VertexComponent component : pLayout.components() ) {
        vertexCode << VERTEX_COMPONENT( component );
    }
    vertexCode << "\n";

    // Source Code
    vertexCode << pVertexSrc;

    // Compile
    string strVertexCode = vertexCode.str();
    const char* vSrc = strVertexCode.c_str();
    LOGGER.log( Level::DEBUG ) << "VertexShader:\n" << vSrc << "\n\n";

    GLuint vertexShaderId = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShaderId, 1, &vSrc, nullptr );
    glCompileShader( vertexShaderId );

    int status;
    glGetShaderiv( vertexShaderId, GL_COMPILE_STATUS, &status );
    if ( status == GL_FALSE ) {

        LOGGER.log( Level::ERROR ) << GET_SHADER_LOG( vertexShaderId ) << "\n";
        throw std::exception();
    }

    return vertexShaderId;
}

GLuint Shader::create_frag_shader( VertexLayout pLayout, vector<Uniform> pFUniforms, vector<TextureSlot> pTexSlots, string pFragSrc )
{
    std::ostringstream fragCode;

    // Header
    fragCode
        << "// Generated by KeSh\n"
        << "#version 330 core\n"
        << "\n";

    // Uniforms
    for ( Uniform uniform : pFUniforms ) {
        fragCode << UNIFORM( uniform );
    }

    // Texture Slots
    for ( TextureSlot slot : pTexSlots ) {
        fragCode << TEXTURE_SLOT( slot );
    }

    // Source Code
    fragCode << pFragSrc;

    // Compile
    string strFragCode = fragCode.str();
    const char* fSrc = strFragCode.c_str();
    LOGGER.log( Level::DEBUG ) << "FragShader:\n" << fSrc << "\n\n";

    GLuint fragShaderId = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragShaderId, 1, &fSrc, nullptr );
    glCompileShader( fragShaderId );

    int status;
    glGetShaderiv( fragShaderId, GL_COMPILE_STATUS, &status );
    if ( status == GL_FALSE ) {

        LOGGER.log(Level::ERROR) << GET_SHADER_LOG( fragShaderId ) << "\n";
        throw std::exception();
    }

    return fragShaderId;
}

GLuint Shader::link_shader( GLuint pVShaderId, GLuint pFShaderId )
{
    int status;
    GLuint shaderId = glCreateProgram();

    glAttachShader( shaderId, pVShaderId );
    glAttachShader( shaderId, pFShaderId );
    glLinkProgram( shaderId );

    glGetProgramiv( shaderId, GL_LINK_STATUS, &status );
    if ( status == GL_FALSE ) {
        throw std::exception( "Shader-Program linking failed!" );
    }
    glValidateProgram( shaderId );

    return shaderId;
}

map<string, Uniform> Shader::process_uniforms( vector<Uniform> pUniforms )
{
    bind();

    map<string, Uniform> uniformMap;
    for ( Uniform uniform : pUniforms ) {
        int32 loc = glGetUniformLocation( _id, uniform.gl_varname().c_str() );
        uniform.gl_location( loc );
        uniformMap[uniform.gl_varname()] = uniform;
    }

    return uniformMap;
}

vector<TextureSlot> Shader::process_textureslots( vector<TextureSlot> pSlots )
{
    bind();

    for ( TextureSlot slot : pSlots ) {
        slot.location = glGetUniformLocation( _id, slot.name.c_str() );
    }

    return pSlots;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                      Private Static                    */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

GLuint Shader::CURRENT_SHADER = 0;

Logger Shader::LOGGER = Logger("Shader", Level::DEBUG);

string Shader::VERTEX_COMPONENT( VertexComponent vComp )
{
    std::ostringstream result;
    result << "layout(location = " << vComp.position << ") in " << vComp.type << " " << vComp.name << ";\n";
    return result.str();
}

string Shader::UNIFORM( Uniform uniform )
{
    std::ostringstream result;
    result << "uniform " << uniform.gl_typename() << " " << uniform.gl_varname() << ";\n";
    return result.str();
}

string Shader::TEXTURE_SLOT( TextureSlot slot )
{
    std::ostringstream result;
    result << "uniform sampler2D " << slot.name << ";\n";
    return result.str();
}

string Shader::GET_SHADER_LOG(GLuint pShaderId )
{
    GLint log_length;
    glGetShaderiv( pShaderId, GL_INFO_LOG_LENGTH, &log_length );
    std::vector<char> v( log_length );
    glGetShaderInfoLog( pShaderId, log_length, NULL, v.data() );
    return std::string( std::begin( v ), std::end( v ) );
}

ENGINE_NAMESPACE_END
