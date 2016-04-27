#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector4Int.hpp"
#include "Engine/Renderer/RGBA.hpp"

struct Vertex_Master;
class ShaderProgram;
//TYPEDEFS//////////////////////////////////////////////////////////////////////////
typedef unsigned char byte;
typedef void (VertexCopyCallback)(const Vertex_Master& source, byte* destination);

//-----------------------------------------------------------------------------------
//The master vertex. This is a superset of all possible vertex data. Used for the mesh builder class.
struct Vertex_Master
{
    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    Vector3 position;
    Vector3 tangent;
    Vector3 bitangent;
    Vector3 normal;
    RGBA color;
    Vector2 uv0;
    Vector2 uv1;
    Vector2 normalizedGlyphPosition;
    Vector2 normalizedStringPosition;
    float normalizedFragPosition;
    Vector4 boneWeights;
    Vector4Int boneIndices;
};

//-----------------------------------------------------------------------------------
struct Vertex_PCT
{
    typedef unsigned int GLuint;

    Vertex_PCT() {};
    Vertex_PCT(const Vector3& position) : pos(position) {};
    Vertex_PCT(const Vector3& position, const RGBA& color) : pos(position), color(color) {};
    Vertex_PCT(const Vector3& position, const RGBA& color, const Vector2& texCoords) : pos(position), color(color), texCoords(texCoords) {};
    static void Copy(const Vertex_Master& source, byte* destination);
    static void BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, ShaderProgram* program);

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    Vector3 pos;
    RGBA color;
    Vector2 texCoords;
};

//-----------------------------------------------------------------------------------
struct Vertex_TextPCT
{
    typedef unsigned int GLuint;

    Vertex_TextPCT() {};
    Vertex_TextPCT(const Vector3& position) : pos(position) {};
    Vertex_TextPCT(const Vector3& position, const RGBA& color) : pos(position), color(color) {};
    Vertex_TextPCT(const Vector3& position, const RGBA& color, const Vector2& texCoords) : pos(position), color(color), texCoords(texCoords) {};
    Vertex_TextPCT(const Vector3& position, const RGBA& color, const Vector2& texCoords, const Vector2& normalizedGlyphPosition, const Vector2 normalizedStringPosition) 
        : pos(position)
        , color(color)
        , texCoords(texCoords)
        , normalizedGlyphPosition(normalizedGlyphPosition)
        , normalizedStringPosition(normalizedStringPosition) {};
    static void Copy(const Vertex_Master& source, byte* destination);
    static void BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, ShaderProgram* program);

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    Vector3 pos;
    RGBA color;
    Vector2 texCoords;
    Vector2 normalizedGlyphPosition;
    Vector2 normalizedStringPosition;
};


//-----------------------------------------------------------------------------------
struct Vertex_PCUTB
{
    typedef unsigned int GLuint;

    Vertex_PCUTB() {};
    Vertex_PCUTB(const Vector3& position) 
        : pos(position) {};
    Vertex_PCUTB(const Vector3& position, const RGBA& color) 
        : pos(position)
        , color(color) {};
    Vertex_PCUTB(const Vector3& position, const RGBA& color, const Vector2& texCoords) 
        : pos(position)
        , color(color)
        , texCoords(texCoords) {};
    Vertex_PCUTB(const Vector3& position, const RGBA& color, const Vector2& texCoords, const Vector3& tangent, const Vector3& bitangent) 
        : pos(position)
        , color(color)
        , texCoords(texCoords)
        , tangent(tangent)
        , bitangent(bitangent) {};
    static void Copy(const Vertex_Master& source, byte* destination);
    static void BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, ShaderProgram* program);

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    Vector3 pos;
    RGBA color;
    Vector2 texCoords;
    Vector3 tangent;
    Vector3 bitangent;
};