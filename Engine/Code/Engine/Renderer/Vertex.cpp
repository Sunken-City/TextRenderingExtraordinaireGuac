#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Engine/Renderer/OpenGLExtensions.hpp"

void Vertex_PCUTB::Copy(const Vertex_Master& source, byte* destination)
{
	Vertex_PCUTB* pctub = (Vertex_PCUTB*)(destination);
	pctub->pos = source.position;
	pctub->color = source.color;
	pctub->texCoords = source.uv0;
	pctub->tangent = source.tangent;
	pctub->bitangent = source.bitangent;
}

void Vertex_PCT::Copy(const Vertex_Master& source, byte* destination)
{
	Vertex_PCT* pct = (Vertex_PCT*)(destination);
	pct->pos = source.position;
	pct->color = source.color;
	pct->texCoords = source.uv0;
}

void Vertex_PCT::BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, ShaderProgram* program)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	program->ShaderProgramBindProperty("inPosition", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_PCT), offsetof(Vertex_PCT, pos));
	program->ShaderProgramBindProperty("inColor", 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex_PCT), offsetof(Vertex_PCT, color));
	program->ShaderProgramBindProperty("inUV0", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PCT), offsetof(Vertex_PCT, texCoords));
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	if (ibo != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}
	glBindVertexArray(NULL);
}

void Vertex_PCUTB::BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, ShaderProgram* program)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	program->ShaderProgramBindProperty("inPosition", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, pos));
	program->ShaderProgramBindProperty("inColor", 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, color));
	program->ShaderProgramBindProperty("inUV0", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, texCoords));
	program->ShaderProgramBindProperty("inTangent", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, tangent));
	program->ShaderProgramBindProperty("inBitangent", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, bitangent));
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	if (ibo != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}
	glBindVertexArray(NULL);
}