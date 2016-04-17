#pragma once
#include <vector>
#include "Engine/Renderer/RGBA.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"

class Vector3Int;
class Vector3;
class ShaderProgram;
class Material;
struct Vertex_PCUTB;


class Mesh
{
	typedef unsigned int GLuint;
public:
	typedef void (BindMeshToVAOForVertex)(GLuint vao, GLuint vbo, GLuint ibo, ShaderProgram* program);
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	Mesh();
	~Mesh();

	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void RenderFromIBO(GLuint vaoID, const Material& program) const;
//	void FlipVs();
// 	static Mesh* CreateCube(float sideLength, const RGBA& color = RGBA::WHITE);
// 	static Mesh* CreateUVSphere(float radius, int numSegments, const RGBA& color = RGBA::WHITE);
// 	static Mesh* CreateIcoSphere(float radius, const RGBA& color = RGBA::WHITE, int numPasses = 3);
// 	static Mesh* CreateQuad(const Vector3& bottomLeft, const Vector3& up, float upLength, const Vector3& right, float rightLength, const RGBA& color = RGBA::WHITE);

	//HELPER FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void Init(void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, void* indexData, unsigned int numIndices, BindMeshToVAOForVertex* BindMeshFunction);
	void BindToVAO(GLuint m_vaoID, ShaderProgram* m_shaderProgram);
// 	static void AddQuadToMesh(Mesh* meshBeingConstructed, const Vector3& bottomLeft, const Vector3& up, float upLength, const Vector3& right, float rightLength, const RGBA& color = RGBA::WHITE, const Vector2& uvOffset = Vector2::ZERO, float uvStepSize = 1.0f);
// 	
	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	GLuint m_vbo;
	GLuint m_ibo;
	unsigned int m_numVerts;
	unsigned int m_numIndices;
	BindMeshToVAOForVertex* m_vertexBindFunctionPointer;
	Renderer::DrawMode m_drawMode;
};