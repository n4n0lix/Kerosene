// Header
#include "shaderbuilder.h"
ENGINE_NAMESPACE_BEGIN

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Public                         */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ShaderBuilder::ShaderBuilder()
{

}

ShaderBuilder& ShaderBuilder::vertexlayout(shared_ptr<VertexLayout> layout)
{
    _vertexLayout = layout;
    return *this;
}

ShaderBuilder& ShaderBuilder::vertex_uniform(string type, string name)
{
    _vsUniforms.add({ type, name });
    return *this;
}

ShaderBuilder& ShaderBuilder::vertex_uniform(UniformTemplate uniformTemplate)
{
    _vsUniforms.add(uniformTemplate);
    return *this;
}

ShaderBuilder& ShaderBuilder::frag_uniform(string type, string name)
{
    _fsUniforms.add({ type, name });
    return *this;
}

ShaderBuilder& ShaderBuilder::frag_texture_slot(TextureSlotTemplate slot)
{
    _fsTextureSlots.add(slot);
    return *this;
}

ShaderBuilder& ShaderBuilder::vertex_source(string vscode)
{
    _vsSource = vscode;
    return *this;
}

ShaderBuilder& ShaderBuilder::frag_source(string fscode)
{
    _fsSource = fscode;
    return *this;
}

owner<Shader> ShaderBuilder::build() const
{
    owner<Shader> shader(new Shader());

    // SHADER CODE
    GLuint vertexShaderId = create_shader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = create_shader(GL_FRAGMENT_SHADER);
    shader->_id = link_program(vertexShaderId, fragmentShaderId);

    // SHADER UNIFORMS
    shader->_vertexUniforms = process_uniforms(shader->_id, &_vsUniforms);
    shader->_fragUniforms   = process_uniforms(shader->_id, &_fsUniforms);
    shader->_fragTextureSlots   = process_texture_slots(shader->_id, &_fsTextureSlots);

    // SHADER LAYOUT
    shader->_vertexLayout = _vertexLayout;

    // CLEAN UP
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return std::move(shader);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Private                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

string ShaderBuilder::gen_vertex_source() const
{
    ostringstream result;

    // Header
    result
        << "// Generated by KeSh\n"
        << "#version 330 core\n"
        << "\n";

    // Uniforms
    for (UniformTemplate uniform : _vsUniforms) {
        result << UNIFORM(uniform);
    }

    // Vertex Components
    for (VertexComponent component : _vertexLayout->components) {
        result << VERTEX_COMPONENT(component);
    }
    result << "\n";

    // Source Code
    result << _vsSource;

    result.flush();
    return result.str();
}

string ShaderBuilder::gen_fragment_source() const
{
    ostringstream result;

    // Header
    result
        << "// Generated by KeSh\n"
        << "#version 330 core\n"
        << "\n";

    // Uniforms
    for (UniformTemplate uniform : _fsUniforms) {
        result << UNIFORM(uniform);
    }

    // Texture Slots
    for (TextureSlotTemplate slot : _fsTextureSlots) {
        result << TEXTURE_SLOT(slot);
    }

    // Source Code
    result << _fsSource;

    result.flush();
    return result.str();
}

GLuint ShaderBuilder::create_shader(GLenum shaderType) const
{
    int status;
    string fsSourceStr;

    // Generate the shader source
    if (shaderType == GL_FRAGMENT_SHADER) {
        fsSourceStr = gen_fragment_source();
    }
    else if (shaderType == GL_VERTEX_SHADER) {
        fsSourceStr = gen_vertex_source();
    }
    else {
        return 0;
    }

    const char* src = fsSourceStr.c_str();
    std::cout << "Shader:\n" << src << "\n\n";

    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        throw EngineException("Shader compiling failed!");
    }

    return shaderId;
}

GLuint ShaderBuilder::link_program(GLuint vertexShaderId, GLuint fragmentShaderId) const
{
    int status;
    GLuint shaderId = glCreateProgram();

    glAttachShader(shaderId, vertexShaderId);
    glAttachShader(shaderId, fragmentShaderId);
    glLinkProgram(shaderId);

    glGetProgramiv(shaderId, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        throw EngineException("Shader-Program linking failed!");
    }
    glValidateProgram(shaderId);

    return shaderId;
}

list<Uniform> ShaderBuilder::process_uniforms(GLuint shaderId, const list<UniformTemplate>* tmplates) const
{
    list<Uniform> uniforms;
    Shader::bind( shaderId );

    for (UniformTemplate tmplate : *tmplates) {
        Uniform uniform = tmplate.to_uniform();
        int32 uniformLocation = glGetUniformLocation(shaderId, tmplate.name.c_str());
        uniforms.add( uniform );
    }

    return uniforms;
}

list<TextureSlot> ShaderBuilder::process_texture_slots(GLuint shaderId, const list<TextureSlotTemplate>* tmplates) const
{
    list<TextureSlot> slots;
    Shader::bind(shaderId);

    for (TextureSlotTemplate tmplate : *tmplates) {
        TextureSlot slot = tmplate.to_textureslot();
        slot.location = glGetUniformLocation(shaderId, tmplate.name.c_str());
        slots.add(slot);
    }

    return slots;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                      Private Static                    */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

string ShaderBuilder::VERTEX_COMPONENT(VertexComponent vComp)
{
    ostringstream result;
    result << "layout(location = " << vComp.position << ") in " << vComp.type << " " << vComp.name << ";\n";
    return result.str();
}

string ShaderBuilder::UNIFORM(UniformTemplate uniformTemplate)
{
    ostringstream result;
    result << "uniform " << uniformTemplate.type << " " << uniformTemplate.name << ";\n";
    return result.str();
}

string ShaderBuilder::TEXTURE_SLOT(TextureSlotTemplate slot)
{
    ostringstream result;
    result << "uniform sampler2D " << slot.name << ";\n";
    return result.str();
}

ENGINE_NAMESPACE_END