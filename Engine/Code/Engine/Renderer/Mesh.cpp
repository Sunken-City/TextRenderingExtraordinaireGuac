#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Math/Vector3Int.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Engine/Renderer/OpenGLExtensions.hpp"

//-----------------------------------------------------------------------------------
Mesh::Mesh()
    : m_drawMode(Renderer::DrawMode::TRIANGLES)
	, m_vbo(0)
	, m_ibo(0)
{

}

//-----------------------------------------------------------------------------------
Mesh::~Mesh()
{
	if (m_vbo != 0)
	{
		Renderer::instance->DeleteBuffers(m_vbo);
	}
	if (m_ibo != 0)
	{
		Renderer::instance->RenderBufferDestroy(m_ibo);
	}
}

// -----------------------------------------------------------------------------------
// Mesh* Mesh::CreateCube(float sideLength, const RGBA& color /*= RGBA::WHITE*/)
// {
//     const float halfSideLength = sideLength / 2.0f;
//     Mesh* meshToCreate = new Mesh();
//    
//     AddQuadToMesh(meshToCreate, Vector3::UP * sideLength, Vector3::FORWARD, sideLength, Vector3::RIGHT, sideLength, color);											//TOP
//     AddQuadToMesh(meshToCreate, Vector3::FORWARD * sideLength, -Vector3::FORWARD, sideLength, Vector3::RIGHT, sideLength, color);									//BOTTOM
//     AddQuadToMesh(meshToCreate, Vector3::ZERO, Vector3::UP, sideLength, Vector3::RIGHT, sideLength, color);															//SOUTH
//     AddQuadToMesh(meshToCreate, Vector3::FORWARD * sideLength + Vector3::RIGHT * sideLength, Vector3::UP, sideLength, -Vector3::RIGHT, sideLength, color);			//NORTH
//     AddQuadToMesh(meshToCreate, Vector3::FORWARD * sideLength, Vector3::UP, sideLength, -Vector3::FORWARD, sideLength, color);										//WEST
//     AddQuadToMesh(meshToCreate, Vector3::RIGHT * sideLength, Vector3::UP, sideLength, Vector3::FORWARD, sideLength, color);											//EAST
// 
//     meshToCreate->Init();
//     return meshToCreate;
// }
// 
// -----------------------------------------------------------------------------------
// Based off of code from http://stackoverflow.com/a/13846592/2619871
// Mesh* Mesh::CreateUVSphere(float radius, int numSegments, const RGBA& color /*= RGBA::WHITE*/)
// {
//     Mesh* meshToCreate = new Mesh();
//     Vertex_PCUTB currentWorkingVertex;
//     currentWorkingVertex.color = color;
//     const float radiansPerSide = MathUtils::HALF_PI / static_cast<float>(numSegments);
// 
//     float const normalizedPhiStepSize = 1.0f / (float)(numSegments - 1);
//     float const normalizedThetaStepSize = 1.0f / (float)(numSegments - 1);
// 
//     for (int currentLatitude = 0; currentLatitude < numSegments; ++currentLatitude) 
//     {
//         for (int currentLongitude = 0; currentLongitude < numSegments; ++currentLongitude) 
//         {
//             float const stepPhi = (-MathUtils::HALF_PI) + (MathUtils::PI * currentLatitude * normalizedPhiStepSize);
//             float const stepTheta = MathUtils::TWO_PI * currentLongitude * normalizedThetaStepSize;
//             float const y = sin(stepPhi);
//             float const tangentY = 0.0f;
//             float const bitangentY = cos(stepPhi);
// 
//             float const x = cos(stepTheta) * cos(stepPhi);
//             float const tangentX = -sin(stepTheta) * cos(stepPhi);
//             float const bitangentX = cos(stepTheta) * -sin(stepPhi);
// 
//             float const z = sin(stepTheta) * cos(stepPhi);
//             float const tangentZ = cos(stepTheta) * cos(stepPhi);
//             float const bitangentZ = sin(stepTheta) * -sin(stepPhi);
// 
//             currentWorkingVertex.texCoords = Vector2(currentLongitude * normalizedThetaStepSize, 1.0f - currentLatitude * normalizedPhiStepSize);
//             currentWorkingVertex.pos = Vector3(x, y, z) * radius;
//             currentWorkingVertex.tangent = Vector3(tangentX, tangentY, tangentZ);
//             currentWorkingVertex.bitangent = Vector3(bitangentX, bitangentY, bitangentZ);
// 
//             meshToCreate->m_verts.push_back(currentWorkingVertex);
// 
//             int currentRow = currentLatitude * numSegments;
//             int nextRow = (currentLatitude + 1) * numSegments;
// 
//             meshToCreate->m_indices.push_back(nextRow + (currentLongitude + 1));
//             meshToCreate->m_indices.push_back(nextRow + currentLongitude);
//             meshToCreate->m_indices.push_back(currentRow + currentLongitude);
// 						
//             meshToCreate->m_indices.push_back(currentRow + (currentLongitude + 1));
//             meshToCreate->m_indices.push_back(nextRow + (currentLongitude + 1));
//             meshToCreate->m_indices.push_back(currentRow + currentLongitude);
//         }
//     }
//     
//     meshToCreate->Init();
//     return meshToCreate;
// }
// 
// -----------------------------------------------------------------------------------
// Mesh* Mesh::CreateIcoSphere(float radius, const RGBA& color /*= RGBA::WHITE*/, int numPasses /*= 3*/)
// {
//     Mesh* meshToCreate = new Mesh();
//     Vector3 initialPoints[6] = { { 0, 0, radius },{ 0, 0, -radius },{ -radius, -radius, 0 },{ radius, -radius, 0 },{ radius, radius, 0 },{ -radius,  radius, 0 } };
//     Vector2 initialUVs[6] = { {0.5f, 0.5f}, {0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f },{ 1.0f, 0.0f } };
//     Vertex_PCUTB currentWorkingVertex;
//     currentWorkingVertex.color = color;
// 
//     for (int i = 0; i < 6; i++)
//     {
//         currentWorkingVertex.pos = Vector3::GetNormalized(initialPoints[i]) * radius;
//         currentWorkingVertex.texCoords = initialUVs[i];
//         meshToCreate->m_verts.push_back(currentWorkingVertex);
//     }
// 
//     meshToCreate->m_indices.push_back(0); meshToCreate->m_indices.push_back(3); meshToCreate->m_indices.push_back(4);
//     meshToCreate->m_indices.push_back(0); meshToCreate->m_indices.push_back(4); meshToCreate->m_indices.push_back(5);
//     meshToCreate->m_indices.push_back(0); meshToCreate->m_indices.push_back(5); meshToCreate->m_indices.push_back(2);
//     meshToCreate->m_indices.push_back(0); meshToCreate->m_indices.push_back(2); meshToCreate->m_indices.push_back(3);
//     meshToCreate->m_indices.push_back(1); meshToCreate->m_indices.push_back(4); meshToCreate->m_indices.push_back(3);
//     meshToCreate->m_indices.push_back(1); meshToCreate->m_indices.push_back(5); meshToCreate->m_indices.push_back(4);
//     meshToCreate->m_indices.push_back(1); meshToCreate->m_indices.push_back(2); meshToCreate->m_indices.push_back(5);
//     meshToCreate->m_indices.push_back(1); meshToCreate->m_indices.push_back(3); meshToCreate->m_indices.push_back(2);
// 
//     for (int i = 0; i < numPasses; i++)
//     {
//         int numberOfFaces = meshToCreate->m_indices.size() / 3;
//         int indicesIndex = 0;
//         for (int j = 0; j < numberOfFaces; j++)
//         {
//             //Get first 3 indices
//             const int x = indicesIndex++;
//             const int y = indicesIndex++;
//             const int z = indicesIndex++;
//             // Calculate the midpoints 
//             Vector3 point1 = Vector3::GetMidpoint(meshToCreate->m_verts.at(meshToCreate->m_indices.at(x)).pos, meshToCreate->m_verts.at(meshToCreate->m_indices.at(y)).pos);
//             Vector3 point2 = Vector3::GetMidpoint(meshToCreate->m_verts.at(meshToCreate->m_indices.at(y)).pos, meshToCreate->m_verts.at(meshToCreate->m_indices.at(z)).pos);
//             Vector3 point3 = Vector3::GetMidpoint(meshToCreate->m_verts.at(meshToCreate->m_indices.at(z)).pos, meshToCreate->m_verts.at(meshToCreate->m_indices.at(x)).pos);
// 
//             //Calculate UV midpoints
//             Vector2 uvPoint1 = Vector2::GetMidpoint(meshToCreate->m_verts.at(meshToCreate->m_indices.at(x)).texCoords, meshToCreate->m_verts.at(meshToCreate->m_indices.at(y)).texCoords);
//             Vector2 uvPoint2 = Vector2::GetMidpoint(meshToCreate->m_verts.at(meshToCreate->m_indices.at(y)).texCoords, meshToCreate->m_verts.at(meshToCreate->m_indices.at(z)).texCoords);
//             Vector2 uvPoint3 = Vector2::GetMidpoint(meshToCreate->m_verts.at(meshToCreate->m_indices.at(z)).texCoords, meshToCreate->m_verts.at(meshToCreate->m_indices.at(x)).texCoords);
// 
//             //Move the points to the outside of our sphere.
//             point1.Normalize();
//             point2.Normalize();
//             point3.Normalize();
// 
//             //Add these vertices to the list of verts, and store their array location.
//             //Naiive way to check for dupes is here. I'll find a better solution later.
//             int point1Location = -1;
//             int point2Location = -1;
//             int point3Location = -1;
//             for (unsigned int q = 0; q < meshToCreate->m_verts.size(); q++)
//             {
//                 Vector3 compare = meshToCreate->m_verts.at(q).pos;
//                 if (compare == point1)
//                     point1Location = q;
//                 if (compare == point2)
//                     point2Location = q;
//                 if (compare == point3)
//                     point3Location = q;
//             }
//             if (point1Location == -1)
//             {
//                 point1Location = meshToCreate->m_verts.size();
//                 currentWorkingVertex.pos = point1 * radius;
//                 currentWorkingVertex.texCoords = uvPoint1;
//                 meshToCreate->m_verts.push_back(currentWorkingVertex);
//             }
// 
//             if (point2Location == -1)
//             {
//                 point2Location = meshToCreate->m_verts.size();
//                 currentWorkingVertex.pos = point2 * radius;
//                 currentWorkingVertex.texCoords = uvPoint2;
//                 meshToCreate->m_verts.push_back(currentWorkingVertex);
//             }
// 
//             if (point3Location == -1)
//             {
//                 point3Location = meshToCreate->m_verts.size();
//                 currentWorkingVertex.pos = point3 * radius;
//                 currentWorkingVertex.texCoords = uvPoint3;
//                 meshToCreate->m_verts.push_back(currentWorkingVertex);
//             }
// 
//             //Create 3 new faces (the outer triangles, the pieces of the triforce)
//             meshToCreate->m_indices.push_back(meshToCreate->m_indices.at(x)); meshToCreate->m_indices.push_back(point1Location);				 meshToCreate->m_indices.push_back(point3Location);
//             meshToCreate->m_indices.push_back(point1Location);				  meshToCreate->m_indices.push_back(meshToCreate->m_indices.at(y)); meshToCreate->m_indices.push_back(point2Location);
//             meshToCreate->m_indices.push_back(point3Location);				  meshToCreate->m_indices.push_back(point2Location);				  meshToCreate->m_indices.push_back(meshToCreate->m_indices.at(z));
// 
//             //Replace the original face with the inner, upside-down triangle (not the triforce)
//             meshToCreate->m_indices.at(x) = point1Location;
//             meshToCreate->m_indices.at(y) = point2Location;
//             meshToCreate->m_indices.at(z) = point3Location;
//         }
//     }
// 
//     meshToCreate->Init();
//     return meshToCreate;
// }
// 
// -----------------------------------------------------------------------------------
// Mesh* Mesh::CreateQuad(const Vector3& bottomLeft, const Vector3& up, float upLength, const Vector3& right, float rightLength, const RGBA& color /*= RGBA::WHITE*/)
// {
//     Mesh* meshToCreate = new Mesh();
// 
//     AddQuadToMesh(meshToCreate, bottomLeft, up, upLength, right, rightLength, color);
// 
//     meshToCreate->Init();
//     return meshToCreate;
// }
// 
// ----------------------------------------------------------------------------------
// void Mesh::AddQuadToMesh(
//     Mesh* meshBeingConstructed, 
//     const Vector3& bottomLeft, 
//     const Vector3& up, 
//     float upLength, 
//     const Vector3& right, 
//     float rightLength, 
//     const RGBA& color /*= RGBA::WHITE*/, 
//     const Vector2& uvOffset /*= Vector2::ZERO*/, 
//     float uvStepSize /*= 1.0f*/
//     )
// {
//     unsigned int currentVert = meshBeingConstructed->m_verts.size();
//     meshBeingConstructed->m_verts.push_back(Vertex_PCUTB(bottomLeft, color, uvOffset + (Vector2::UNIT_Y * uvStepSize), right, up));
//     meshBeingConstructed->m_verts.push_back(Vertex_PCUTB(bottomLeft + (up * upLength), color, uvOffset, right, up));
//     meshBeingConstructed->m_verts.push_back(Vertex_PCUTB(bottomLeft + (right * rightLength), color, uvOffset + (Vector2::ONE * uvStepSize), right, up));
//     meshBeingConstructed->m_verts.push_back(Vertex_PCUTB(bottomLeft + (up * upLength) + (right * rightLength), color, uvOffset + (Vector2::UNIT_X * uvStepSize), right, up));
// 
// 	meshBeingConstructed->m_indices.push_back(2 + currentVert);
// 	meshBeingConstructed->m_indices.push_back(1 + currentVert);
// 	meshBeingConstructed->m_indices.push_back(0 + currentVert);
// 	meshBeingConstructed->m_indices.push_back(2 + currentVert);
// 	meshBeingConstructed->m_indices.push_back(3 + currentVert);
// 	meshBeingConstructed->m_indices.push_back(1 + currentVert);
// }

//-----------------------------------------------------------------------------------
void Mesh::RenderFromIBO(GLuint vaoID, const Material& program) const
{
    glBindVertexArray(vaoID);
    program.SetUpRenderState();
    //Draw with IBO
    glDrawElements(Renderer::instance->GetDrawMode(m_drawMode), m_numIndices, GL_UNSIGNED_INT, (GLvoid*)0);
    program.CleanUpRenderState();
    glUseProgram(NULL);
    glBindVertexArray(NULL);
}

//Pushes data over to the GPU and creates the buffers. The mesh doesn't store any of the vertexes or indexes, just the buffer locations.
//-----------------------------------------------------------------------------------
void Mesh::Init(void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, void* indexData, unsigned int numIndices, BindMeshToVAOForVertex* BindMeshFunction)
{
	m_numVerts = numVertices;
	m_numIndices = numIndices;
	m_vertexBindFunctionPointer = BindMeshFunction;
	m_vbo = Renderer::instance->GenerateBufferID();
	GL_CHECK_ERROR();
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeofVertex * numVertices, vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	GL_CHECK_ERROR();
	m_ibo = Renderer::instance->RenderBufferCreate(indexData, numIndices, sizeof(unsigned int), GL_STATIC_DRAW);
	GL_CHECK_ERROR();
}

//-----------------------------------------------------------------------------------
void Mesh::BindToVAO(GLuint vaoID, ShaderProgram* shaderProgram)
{
	m_vertexBindFunctionPointer(vaoID, m_vbo, m_ibo, shaderProgram);
}

// -----------------------------------------------------------------------------------
// void Mesh::FlipVs()
// {
//     for (unsigned int index = 0; index < m_verts.size(); ++index)
//     {
//         m_verts[index].texCoords.y = 1.0f - m_verts[index].texCoords.y;
//     }
// }
