#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/Console.hpp"
#include "Engine/Input/BinaryWriter.hpp"
#include "Engine/Input/BinaryReader.hpp"
#include "Engine/Renderer/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

extern MeshBuilder* g_loadedMeshBuilder;
extern Mesh* g_loadedMesh;

//-----------------------------------------------------------------------------------
#if defined(TOOLS_BUILD)
CONSOLE_COMMAND(saveMesh)
{
	if (!args.HasArgs(1))
	{
		Console::instance->PrintLine("saveMesh <filename>", RGBA::RED);
		return;
	}
	std::string filename = args.GetStringArgument(0);
	if (!g_loadedMeshBuilder)
	{
		Console::instance->PrintLine("Error: No mesh has been loaded yet, use fbxLoad to bring in a mesh first.", RGBA::RED);
		return;
	}
	g_loadedMeshBuilder->WriteToFile(filename.c_str());
}

CONSOLE_COMMAND(loadMesh)
{
	if (!args.HasArgs(1))
	{
		Console::instance->PrintLine("loadMesh <filename>", RGBA::RED);
		return;
	}
	std::string filename = args.GetStringArgument(0);
	if (g_loadedMeshBuilder)
	{
		delete g_loadedMeshBuilder;
	}
	g_loadedMeshBuilder = new MeshBuilder();
	g_loadedMeshBuilder->ReadFromFile(filename.c_str());
	g_loadedMesh = new Mesh();
	g_loadedMeshBuilder->CopyToMesh(g_loadedMesh, &Vertex_PCUTB::Copy, TODO, TODO);
}
#endif

//-----------------------------------------------------------------------------------
MeshBuilder::MeshBuilder()
	: m_startIndex(0)
	, m_materialName(nullptr)
	, m_dataMask(0)
{

}

//-----------------------------------------------------------------------------------
void MeshBuilder::Begin()
{
	m_startIndex = m_vertices.size();
}

//-----------------------------------------------------------------------------------
void MeshBuilder::End()
{
	if (m_startIndex < m_vertices.size())
	{
		m_startIndex = m_vertices.size();
	}
}

//-----------------------------------------------------------------------------------
MeshBuilder* MeshBuilder::Merge(MeshBuilder* meshBuilderArray, unsigned int numberOfMeshes)
{
	MeshBuilder* combinedMesh = new MeshBuilder();
	for (unsigned int i = 0; i < numberOfMeshes; i++)
	{
		int numPreexistingVerts = combinedMesh->m_indices.size();
		MeshBuilder& currentMesh = meshBuilderArray[i];
		for (Vertex_Master vert : currentMesh.m_vertices)
		{
			combinedMesh->m_vertices.push_back(vert);
		}
		for (unsigned int index : currentMesh.m_indices)
		{
			combinedMesh->m_indices.push_back(index + (i * numPreexistingVerts));
		}
		combinedMesh->m_dataMask |= currentMesh.m_dataMask;
	}
	return combinedMesh;
}

//-----------------------------------------------------------------------------------
void MeshBuilder::CopyToMesh(Mesh* mesh, VertexCopyCallback* copyFunction, unsigned int sizeofVertex, Mesh::BindMeshToVAOForVertex* bindMeshFunction)
{
	// First, we need to allocate a buffer to copy 
	// our vertices into, that matches what the mesh
	// wants.  
	unsigned int vertexCount = m_vertices.size();
	if (vertexCount == 0) {
		// nothing in this mesh.
		return;
	}

	unsigned int vertexSize = sizeofVertex; //mesh->vdefn->vertexSize;
	unsigned int vertex_buffer_size = vertexCount * vertexSize;

	byte* vertexBuffer = new byte[vertex_buffer_size];
	byte* currentBufferIndex = vertexBuffer;

//	mesh->m_verts.clear();
	for (unsigned int vertex_index = 0;	vertex_index < vertexCount;	++vertex_index) 
	{
		copyFunction(m_vertices[vertex_index], currentBufferIndex);
// 		Vertex_PCUTB* vertexPointer = (Vertex_PCUTB*)currentBufferIndex;
// 		mesh->m_verts.push_back(*vertexPointer);
		currentBufferIndex += vertexSize;
	}

	//mesh->m_indices.assign(m_indices.begin(), m_indices.end());
	mesh->Init(vertexBuffer, vertexCount, sizeofVertex, m_indices.data(), m_indices.size(), bindMeshFunction);

	//mesh->vbo.write(vertexBuffer, vertex_buffer_size);
	//mesh->draw_instructions.add_all(draw_instructions);

	// Make sure we clean up after ourselves
	delete vertexBuffer;
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddVertex(const Vector3& position)
{
	m_stamp.position = position;
	m_vertices.push_back(m_stamp);
	SetMaskBit(POSITION_BIT);
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddQuadIndicesClockwise(unsigned int tlIndex, unsigned int trIndex, unsigned int blIndex, unsigned int brIndex)
{
	m_indices.push_back(brIndex);
	m_indices.push_back(blIndex);
	m_indices.push_back(tlIndex);
	m_indices.push_back(brIndex);
	m_indices.push_back(tlIndex);
	m_indices.push_back(trIndex);
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddQuadIndices(unsigned int tlIndex, unsigned int trIndex, unsigned int blIndex, unsigned int brIndex)
{
	m_indices.push_back(brIndex);
	m_indices.push_back(tlIndex);
	m_indices.push_back(blIndex);
	m_indices.push_back(brIndex);
	m_indices.push_back(trIndex);
	m_indices.push_back(tlIndex);
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddQuadIndices()
{
	unsigned int lastIndex = m_vertices.size(); 
	AddQuadIndices(lastIndex + 3, lastIndex + 2, lastIndex + 0, lastIndex + 1);
}

//-----------------------------------------------------------------------------------
void MeshBuilder::BuildQuad(
	const Vector3& initial_position,
	const Vector3& right,
	const Vector3& up,
	float startX, float endX,
	float startY, float endY,
	float startU, float endU,
	float startV, float endV)
{
	this->Begin();

	float const xStep = endX - startX;
	float const yStep = endY - startY;
	float const uStep = endU - startU;
	float const vStep = endV - startV;

	// So, what is our tangent, normal, and bitangent?
	Vector3 tangent = right;
	Vector3 bitangent = up;
	Vector3 normal = Vector3::Cross(bitangent, tangent);
	this->SetTBN(tangent, bitangent, normal);

	// Okay, now, let's add all our vertices
	float x, y;
	float u, v;

	y = startY;
	v = startV;

	for (unsigned int iy = 0; iy < 2; ++iy) {
		x = startX;
		u = startU;

		for (uint32_t ix = 0; ix < 2; ++ix) {
			this->SetUV(u, v);

			Vector3 position = initial_position
				+ (right * x)
				+ (up * y);

			this->AddVertex(position);

			x += xStep;
			u += uStep;
		}

		y += yStep;
		v += vStep;
	}

	this->AddQuadIndices(1, 3, 0, 2);

	this->End();
}

//-----------------------------------------------------------------------------------
void MeshBuilder::BuildPlane(
	const Vector3& initialPosition,
	const Vector3& right,
	const Vector3& up,
	float startX, float endX, uint32_t xSections,
	float startY, float endY, uint32_t ySections)
{
	ASSERT_OR_DIE(xSections > 0, "xSections passed in to BuildPlane has an invalid value");
	ASSERT_OR_DIE(ySections > 0, "ySections passed in to BuildPlane has an invalid value");

	this->Begin();

	//Add vertices to patch
	uint32_t xVertCount = xSections + 1;
	uint32_t yVertCount = ySections + 1;

	float const xRange = endX - startX;
	float const yRange = endY - startY;
	float const xStep = xRange / (float)xSections;
	float const yStep = yRange / (float)ySections;

	//Textures the entire patch
	float const uStep = 1.0f / (float)xSections;
	float const vStep = 1.0f / (float)ySections;

	// So, what is our tangent, normal, and bitangent?
	Vector3 tangent = right;
	Vector3 bitangent = up;
	Vector3 normal = Vector3::Cross(bitangent, tangent);
	this->SetTBN(tangent, bitangent, normal);

	unsigned int startVertexIndex = this->GetCurrentIndex();

	// Okay, now, let's add all our vertices
	float x, y;
	float u, v;

	y = startY;
	v = 0.0f;

	for (unsigned int iy = 0; iy < yVertCount; ++iy) {
		x = startX;
		u = 0.0f;

		for (uint32_t ix = 0; ix < xVertCount; ++ix) {
			this->SetUV(u, v);

			Vector3 position = initialPosition
				+ (right * x)
				+ (up * y);

			this->AddVertex(position);

			x += xStep;
			u += uStep;
		}

		y += yStep;
		v += vStep;
	}

	// Next, add all the indices for this patch
	for (unsigned int iy = 0; iy < ySections; ++iy) {
		for (unsigned int ix = 0; ix < xSections; ++ix) {

			unsigned int blIndex = startVertexIndex
				+ (iy * xVertCount) + ix;
			unsigned int brIndex = blIndex + 1;
			unsigned int tlIndex = blIndex + xVertCount;
			unsigned int trIndex = tlIndex + 1;

			// How many indices is this actually adding
			// under the hood, it we're working with triangles?
			this->AddQuadIndices(tlIndex, trIndex, blIndex, brIndex);
		}
	}

	this->End();
}

//-----------------------------------------------------------------------------------
static Vector3 PlaneFunc(const void* userData, float x, float y)
{
	MeshBuilder::PlaneData const *plane = (MeshBuilder::PlaneData const*)userData;
	Vector3 position = plane->initialPosition
		+ (plane->right * x)
		+ (plane->up * y);
	position.z = sin(x + y);
	return position;
}

//-----------------------------------------------------------------------------------
void MeshBuilder::BuildPlaneFromFunc(
	const Vector3& initialPosition,
	const Vector3& right,
	const Vector3& up,
	float startX, float endX, uint32_t xSections,
	float startY, float endY, uint32_t ySections)
{
	MeshBuilder::PlaneData plane;
	plane.initialPosition = initialPosition;
	plane.right = right;
	plane.up = up;

	this->BuildPatch(
	startX, endX, xSections,
	startY, endY, ySections,
	PlaneFunc,
	&plane);
}

//-----------------------------------------------------------------------------------
void MeshBuilder::BuildPatch(
	float startX, float endX, uint32_t xSections,
	float startY, float endY, uint32_t ySections,
	PatchFunction* patchFunction,
	void *userData)
{
	ASSERT_OR_DIE(xSections > 0, "xSections passed in to BuildPlane has an invalid value");
	ASSERT_OR_DIE(ySections > 0, "ySections passed in to BuildPlane has an invalid value");

	this->Begin();

	uint32_t xVertCount = xSections + 1;
	uint32_t yVertCount = ySections + 1;

	float const xRange = endX - startX;
	float const yRange = endY - startY;
	float const xStep = xRange / (float)xSections;
	float const yStep = yRange / (float)ySections;

	float const uStep = 1.0f / (float)xSections;
	float const vStep = 1.0f / (float)ySections;

	uint32_t startVertIndex = this->GetCurrentIndex();

	//Add in the vertices
	float x, y;
	float u, v;

	y = startY;
	v = 0.0f;

	float const delta = .01f; // artitrarily small value, can go smaller

	for (uint32_t iy = 0; iy < yVertCount; ++iy) {
		x = startX;
		u = 0.0f;

		for (uint32_t ix = 0; ix < xVertCount; ++ix) {
			this->SetUV(u, v);

			// calculate tangent along u (that is, x)
			Vector3 tangent = patchFunction(userData, x + delta, y) - patchFunction(userData, x - delta, y);

			// calculate bitangent along v (taht is, y)
			Vector3 bitangent = patchFunction(userData, x, y + delta) - patchFunction(userData, x, y - delta);

			tangent.Normalize();
			bitangent.Normalize();
			Vector3 normal = Vector3::Cross(bitangent, tangent);
			bitangent = Vector3::Cross(tangent, normal);
			this->SetTBN(tangent, bitangent, normal);

			Vector3 position = patchFunction(userData, x, y);
			this->AddVertex(position);

			x += xStep;
			u += uStep;
		}

		y += yStep;
		v += vStep;
	}

	//Add all the indices for the patch
	for (uint32_t iy = 0; iy < ySections; ++iy) {
		for (uint32_t ix = 0; ix < xSections; ++ix) {

			uint32_t blIdx = startVertIndex
				+ (iy * xVertCount) + ix;
			uint32_t brIdx = blIdx + 1;
			uint32_t tlIdx = blIdx + xVertCount;
			uint32_t trIdx = tlIdx + 1;

			this->AddQuadIndices(tlIdx, trIdx, blIdx, brIdx);
		}
	}
	this->End();
}

//-----------------------------------------------------------------------------------
void MeshBuilder::WriteDataMask(IBinaryWriter& writer)
{
	if ((m_dataMask & (1 << POSITION_BIT)) != 0)
	{
		writer.WriteString("Position");
	}
	if ((m_dataMask & (1 << TANGENT_BIT)) != 0)
	{
		writer.WriteString("Tangent");
	}
	if ((m_dataMask & (1 << BITANGENT_BIT)) != 0)
	{
		writer.WriteString("Bitangent");
	}
	if ((m_dataMask & (1 << NORMAL_BIT)) != 0)
	{
		writer.WriteString("Normal");
	}
	if ((m_dataMask & (1 << COLOR_BIT)) != 0)
	{
		writer.WriteString("Color");
	}
	if ((m_dataMask & (1 << UV0_BIT)) != 0)
	{
		writer.WriteString("UV0");
	}
	if ((m_dataMask & (1 << UV1_BIT)) != 0)
	{
		writer.WriteString("UV1");
	}

	writer.WriteString(nullptr);
}

//-----------------------------------------------------------------------------------
uint32_t MeshBuilder::ReadDataMask(IBinaryReader& reader)
{
	uint32_t mask = 0;
	const char* str = nullptr;
	size_t size = reader.ReadString(str, 64);
	while (size > 0) 
	{
		if (strcmp(str, "Position") == 0)
		{
			mask |= (1 << POSITION_BIT);
		}
		else if (strcmp(str, "Tangent") == 0)
		{
			mask |= (1 << TANGENT_BIT);
		}
		else if (strcmp(str, "Bitangent") == 0)
		{
			mask |= (1 << BITANGENT_BIT);
		}
		else if (strcmp(str, "Normal") == 0)
		{
			mask |= (1 << NORMAL_BIT);
		}
		else if (strcmp(str, "Color") == 0)
		{
			mask |= (1 << COLOR_BIT);
		}
		else if (strcmp(str, "UV0") == 0)
		{
			mask |= (1 << UV0_BIT);
		}
		else if (strcmp(str, "UV1") == 0)
		{
			mask |= (1 << UV1_BIT);
		}

		size = reader.ReadString(str, 64);
	}
	delete str;
	return mask;
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddTexturedAABB(const AABB2& bounds, const Vector2& uvMins, const Vector2& uvMaxs, const RGBA& color)
{
	int startingVertex = m_vertices.size();
	SetColor(color);
	SetUV(uvMins);
	AddVertex(Vector3(bounds.mins.x, bounds.mins.y, 0.0f));
	SetUV(Vector2(uvMaxs.x, uvMins.y));
	AddVertex(Vector3(bounds.maxs.x, bounds.mins.y, 0.0f));
	SetUV(uvMaxs);
	AddVertex(Vector3(bounds.maxs.x, bounds.maxs.y, 0.0f));
	SetUV(Vector2(uvMins.x, uvMaxs.y));
	AddVertex(Vector3(bounds.mins.x, bounds.maxs.y, 0.0f));
	AddQuadIndices(startingVertex + 3, startingVertex + 2, startingVertex + 0, startingVertex + 1);
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddText2D(const Vector2& position, const std::string& asciiText, float scale, const RGBA& tint /*= RGBA::WHITE*/, bool drawShadow /*= false*/, const BitmapFont* font /*= nullptr*/)
{
	if (asciiText.empty())
	{
		return;
	}
	if (font == nullptr)
	{
		font = Renderer::instance->m_defaultFont;
	}
	int stringLength = asciiText.size();
	Vector2 cursorPosition = position + (Vector2::UNIT_Y * (float)font->m_maxHeight * scale);
	const Glyph* previousGlyph = nullptr;
	for (int i = 0; i < stringLength; i++)
	{
		unsigned char currentCharacter = asciiText[i];
		const Glyph* glyph = font->GetGlyph(currentCharacter);
		float glyphWidth = static_cast<float>(glyph->width) * scale;
		float glyphHeight = static_cast<float>(glyph->height) * scale;

		if (previousGlyph)
		{
			const Vector2 kerning = font->GetKerning(*previousGlyph, *glyph);
			cursorPosition += (kerning * scale);
		}
		Vector2 offset = Vector2(glyph->xOffset * scale, -glyph->yOffset * scale);
		Vector2 topRight = cursorPosition + offset + Vector2(glyphWidth, 0.0f);
		Vector2 bottomLeft = cursorPosition + offset - Vector2(0.0f, glyphHeight);
		AABB2 quadBounds = AABB2(bottomLeft, topRight);
		AABB2 glyphBounds = font->GetTexCoordsForGlyph(*glyph);
		if (drawShadow)
		{
			float shadowWidthOffset = glyphWidth / 10.0f;
			float shadowHeightOffset = glyphHeight / -10.0f;
			Vector2 shadowOffset = Vector2(shadowWidthOffset, shadowHeightOffset);
			AABB2 shadowBounds = AABB2(bottomLeft + shadowOffset, topRight + shadowOffset);
			this->AddTexturedAABB(shadowBounds, glyphBounds.mins, glyphBounds.maxs, RGBA::BLACK);
		}
		this->AddTexturedAABB(quadBounds, glyphBounds.mins, glyphBounds.maxs, tint);
		cursorPosition.x += glyph->xAdvance * scale;
		previousGlyph = glyph;
	}
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddIndex(int index)
{
	m_indices.push_back(index);
}

//-----------------------------------------------------------------------------------
void MeshBuilder::AddLinearIndices()
{
	for (unsigned int i = 0; i < m_vertices.size(); ++i)
	{
		AddIndex(i);
	}
}

//-----------------------------------------------------------------------------------
void MeshBuilder::WriteToFile(const char* filename)
{
	BinaryFileWriter writer;
	ASSERT_OR_DIE(writer.Open(filename), "File Open failed!");
	{
		WriteToStream(writer);
	}
	writer.Close();
}

//-----------------------------------------------------------------------------------
void MeshBuilder::WriteToStream(IBinaryWriter& writer)
{
	//FILE VERSION
	//material id
	//vertex data mask (ie: position, tangent, normal, etc...)
	//vertices
	//indices

	writer.Write<uint32_t>(FILE_VERSION);
	writer.WriteString(m_materialName);
	WriteDataMask(writer);
	uint32_t vertexCount = m_vertices.size();
	uint32_t indicesCount = m_indices.size();
	writer.Write<uint32_t>(vertexCount);
	for (Vertex_Master vertex : m_vertices)
	{
		//TODO("Clean this up when you're not running on no sleep, it's not efficient");
		IsInMask(POSITION_BIT) ? writer.Write<Vector3>(vertex.position) : false;
		IsInMask(TANGENT_BIT) ? writer.Write<Vector3>(vertex.tangent) : false;
		IsInMask(BITANGENT_BIT) ? writer.Write<Vector3>(vertex.bitangent) : false;
		IsInMask(NORMAL_BIT) ? writer.Write<Vector3>(vertex.normal) : false;
		IsInMask(COLOR_BIT) ? writer.Write<RGBA>(vertex.color) : false;
		IsInMask(UV0_BIT) ? writer.Write<Vector2>(vertex.uv0) : false;
		IsInMask(UV1_BIT) ? writer.Write<Vector2>(vertex.uv1) : false;
	}
	writer.Write<uint32_t>(indicesCount);
	for (unsigned int index : m_indices)
	{
		writer.Write<unsigned int>(index);
	}
}

void MeshBuilder::ReadFromStream(IBinaryReader& reader)
{
	//FILE VERSION
	//material id
	//vertex data mask (ie: position, tangent, normal, etc...)
	//vertices
	//indices

	uint32_t fileVersion;
	const char* materialName = nullptr;
	uint32_t vertexCount;
	uint32_t indicesCount;

	ASSERT_OR_DIE(reader.Read<uint32_t>(fileVersion), "Failed to read file version");
	reader.ReadString(materialName, 64);
	SetMaterialName(materialName);
	m_dataMask = ReadDataMask(reader);
	ASSERT_OR_DIE(reader.Read<uint32_t>(vertexCount), "Failed to read vertex count");
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		//TODO("Clean this up when you're not running on no sleep, it's not efficient");
		Vertex_Master vertex;
		IsInMask(POSITION_BIT) ? reader.Read<Vector3>(vertex.position) : false;
		IsInMask(TANGENT_BIT) ? reader.Read<Vector3>(vertex.tangent) : false;
		IsInMask(BITANGENT_BIT) ? reader.Read<Vector3>(vertex.bitangent) : false;
		IsInMask(NORMAL_BIT) ? reader.Read<Vector3>(vertex.normal) : false;
		IsInMask(COLOR_BIT) ? reader.Read<RGBA>(vertex.color) : false;
		IsInMask(UV0_BIT) ? reader.Read<Vector2>(vertex.uv0) : false;
		IsInMask(UV1_BIT) ? reader.Read<Vector2>(vertex.uv1) : false;
		m_vertices.push_back(vertex);
	}	
	ASSERT_OR_DIE(reader.Read<uint32_t>(indicesCount), "Failed to read index count");
	for (unsigned int i = 0; i < indicesCount; ++i)
	{
		unsigned int index;
		reader.Read<unsigned int>(index);
		m_indices.push_back(index);
	}
}

void MeshBuilder::ReadFromFile(const char* filename)
{
	BinaryFileReader reader;
	ASSERT_OR_DIE(reader.Open(filename), "File Open failed!");
	{
		ReadFromStream(reader);
	}
	reader.Close();
}
