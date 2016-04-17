#include "Engine/Tools/fbx.hpp"
#include "Engine/Input/Console.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MatrixStack4x4.hpp"
#include "Engine/Renderer/Mesh.hpp"

Mesh* g_loadedMesh = nullptr;
MeshBuilder* g_loadedMeshBuilder = nullptr;

#if defined(TOOLS_BUILD)
	//For tools only
	#include "ThirdParty/FBX/include/fbxsdk.h"
#include "../Renderer/Skeleton.hpp"
	#pragma comment(lib, "libfbxsdk-md.lib")

	//-----------------------------------------------------------------------------------
	CONSOLE_COMMAND(fbxList)
	{
		std::string filename = args.GetStringArgument(0);
		FbxListScene(filename.c_str());
	}

	//-----------------------------------------------------------------------------------
	CONSOLE_COMMAND(fbxLoad)
	{
		if (!(args.HasArgs(2) || args.HasArgs(1)))
		{
			Console::instance->PrintLine("fbxLoad <file path> <scale>", RGBA::RED);
			return;
		}
		std::string filename = args.GetStringArgument(0);

		float scale = args.HasArgs(2) ? args.GetFloatArgument(1) : 1.0f;
		Matrix4x4 transform;
		Matrix4x4::MatrixMakeScale(&transform, scale);
		
		SceneImport* import = FbxLoadSceneFromFile(filename.c_str(), Matrix4x4::IDENTITY, false, transform);
		if (import == nullptr)
		{
			Console::instance->PrintLine(Stringf("Failed to load file. '%s'", filename.c_str()));
			DebuggerPrintf("Failed to load file. '%s'", filename.c_str());
		}
		else
		{
			Console::instance->PrintLine(Stringf("Loaded '%s'. Had %i meshes.", filename.c_str(), import->meshes.size()));
			DebuggerPrintf("Loaded '%s'. Had %i meshes.", filename.c_str(), import->meshes.size());
			g_loadedMesh = new Mesh();
			g_loadedMeshBuilder = MeshBuilder::Merge(import->meshes.data(), import->meshes.size());
			g_loadedMeshBuilder->AddLinearIndices();
			g_loadedMeshBuilder->CopyToMesh(g_loadedMesh, &Vertex_PCUTB::Copy, sizeof(Vertex_PCUTB), TODO);
		}
		delete import;
	}

	//-----------------------------------------------------------------------------------
	static Vector3 ToEngineVec3(const FbxVector4& fbxVec4)
	{
		return Vector3((float)fbxVec4.mData[0],
			(float)fbxVec4.mData[1],
			(float)fbxVec4.mData[2]);
	}

	//-----------------------------------------------------------------------------------
	static Vector4 ToEngineVec4(const FbxDouble4& fbxVec)
	{
		return Vector4(
			static_cast<float>(fbxVec.mData[0]),
			static_cast<float>(fbxVec.mData[1]),
			static_cast<float>(fbxVec.mData[2]),
			static_cast<float>(fbxVec.mData[3])
			);
	}

	//-----------------------------------------------------------------------------------
	static RGBA ToEngineRGBA(const FbxColor& fbxColor)
	{
		return RGBA(
			static_cast<float>(fbxColor.mRed),
			static_cast<float>(fbxColor.mBlue),
			static_cast<float>(fbxColor.mGreen),
			static_cast<float>(fbxColor.mAlpha)
			);
	}

	//-----------------------------------------------------------------------------------
	static Matrix4x4 ToEngineMatrix(const FbxMatrix& fbxMat)
	{
		Matrix4x4 mat;
		Matrix4x4::MatrixSetRows(&mat,
			ToEngineVec4(fbxMat.mData[0]),
			ToEngineVec4(fbxMat.mData[1]),
			ToEngineVec4(fbxMat.mData[2]),
			ToEngineVec4(fbxMat.mData[3]));
		return mat;
	}

	//-----------------------------------------------------------------------------------
	static const char* GetAttributeTypeName(FbxNodeAttribute::EType type)
	{
		switch (type) 
		{ 
			case FbxNodeAttribute::eUnknown: return "unidentified";            
			case FbxNodeAttribute::eNull: return "null";            
			case FbxNodeAttribute::eMarker: return "marker";           
			case FbxNodeAttribute::eSkeleton: return "skeleton";            
			case FbxNodeAttribute::eMesh: return "mesh";            
			case FbxNodeAttribute::eNurbs: return "nurbs";          
			case FbxNodeAttribute::ePatch: return "patch";          
			case FbxNodeAttribute::eCamera: return "camera";          
			case FbxNodeAttribute::eCameraStereo: return "stereo";  
			case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";  
			case FbxNodeAttribute::eLight: return "light";         
			case FbxNodeAttribute::eOpticalReference: return "optical reference";   
			case FbxNodeAttribute::eOpticalMarker: return "marker";          
			case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";        
			case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";         
			case FbxNodeAttribute::eBoundary: return "boundary";          
			case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";        
			case FbxNodeAttribute::eShape: return "shape";           
			case FbxNodeAttribute::eLODGroup: return "lodgroup";       
			case FbxNodeAttribute::eSubDiv: return "subdiv";            
			default: return "unknown"; 
		}
	}

	//-----------------------------------------------------------------------------------
	static void PrintAttribute(FbxNodeAttribute* attribute, int depth)
	{
		if (attribute == nullptr)
		{
			return;
		}

		FbxNodeAttribute::EType type = attribute->GetAttributeType();
		const char* typeName = GetAttributeTypeName(type);
		const char* attribName = attribute->GetName();

		Console::instance->PrintLine(Stringf("%*s- type='%s', name='%s'\n", depth, " ", typeName, attribName));
		DebuggerPrintf("%*s- type='%s', name='%s'\n", depth, " ", typeName, attribName);
	}

	//-----------------------------------------------------------------------------------
	static void PrintNode(FbxNode* node, int depth)
	{
		Console::instance->PrintLine(Stringf("%*sNode [%s]\n", depth, " ", node->GetName()));
		DebuggerPrintf("%*sNode [%s]\n", depth, " ", node->GetName());
		for (int i = 0; i < node->GetNodeAttributeCount(); ++i)
		{
			PrintAttribute(node->GetNodeAttributeByIndex(i), depth);
		}
		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			PrintNode(node->GetChild(i), depth + 1);
		}
	}

	//-----------------------------------------------------------------------------------
	void FbxListScene(const char* filename)
	{
		FbxManager* fbxManager = FbxManager::Create();
		if (nullptr == fbxManager)
		{
			Console::instance->PrintLine("Could not create fbx manager.");
			DebuggerPrintf("Could not create fbx manager.");
			return;
		}
		
		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT); //Name of object is blank, we don't care
		fbxManager->SetIOSettings(ioSettings);

		//Create an importer
		FbxImporter* importer = FbxImporter::Create(fbxManager, "");

		bool loadSuccessful = importer->Initialize(filename, -1, fbxManager->GetIOSettings());
		if (loadSuccessful)
		{
			//We have imported the FBX
			FbxScene* scene = FbxScene::Create(fbxManager, "");
			bool importSuccessful = importer->Import(scene);
			if (importSuccessful)
			{
				FbxNode* root = scene->GetRootNode();
				PrintNode(root, 0);
			}

			FBX_SAFE_DESTROY(scene);
		}
		else
		{
			Console::instance->PrintLine(Stringf("Could not import scene: %s", filename));
			DebuggerPrintf("Could not import scene: %s", filename);
		}

		FBX_SAFE_DESTROY(importer);
		FBX_SAFE_DESTROY(ioSettings);
		FBX_SAFE_DESTROY(fbxManager);
	}

	//-----------------------------------------------------------------------------------
	static bool GetPosition(Vector3* outPosition, const Matrix4x4& transform, FbxMesh* mesh, int polyIndex, int vertIndex)
	{
		FbxVector4 fbxPosition;
		int controlIndex = mesh->GetPolygonVertex(polyIndex, vertIndex);
		fbxPosition = mesh->GetControlPointAt(controlIndex);
		*outPosition = Vector3(Vector4(ToEngineVec3(fbxPosition), 1.0f) * transform);
		return true;
	}

	//-----------------------------------------------------------------------------------
	template <typename ElemType, typename VarType>
	static bool GetObjectFromElement(FbxMesh* mesh, int polyIndex, int vertIndex, ElemType* elem, VarType* outVar)
	{
		if (nullptr == elem)
		{
			return false;
		}
		switch(elem->GetMappingMode())
		{
			case FbxGeometryElement::eByControlPoint:
			{
				int controlIndex = mesh->GetPolygonVertex(polyIndex, vertIndex);
				switch (elem->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					if (controlIndex < elem->GetDirectArray().GetCount())
					{
						*outVar = elem->GetDirectArray().GetAt(controlIndex);
						return true;
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					if (controlIndex < elem->GetIndexArray().GetCount())
					{
						int index = elem->GetIndexArray().GetAt(controlIndex);
						*outVar = elem->GetDirectArray().GetAt(index);
						return true;
					}
					break;
				default:
					break;
				}
			}
			break;
			case FbxGeometryElement::eByPolygonVertex:
			{
				int directVertexIndex = (polyIndex * 3) + vertIndex;
				switch (elem->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					if (directVertexIndex < elem->GetDirectArray().GetCount())
					{
						*outVar = elem->GetDirectArray().GetAt(directVertexIndex);
						return true;
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					if (directVertexIndex < elem->GetIndexArray().GetCount())
					{
						int index = elem->GetIndexArray().GetAt(directVertexIndex);
						*outVar = elem->GetDirectArray().GetAt(index);
						return true;
					}
					break;
				default:
					break;
				}
			}
			break;
			default:
				ERROR_AND_DIE("Undefined Mapping Mode")
				break;
		}
		return false;
	}

	//-----------------------------------------------------------------------------------
	static bool GetNormal(Vector3& outNormal, const Matrix4x4& transform, FbxMesh* mesh, int polyIndex, int vertIndex)
	{
		FbxVector4 normal;
		FbxGeometryElementNormal* normals = mesh->GetElementNormal(0);
		if (GetObjectFromElement(mesh, polyIndex, vertIndex, normals, &normal))
		{
			Vector3 n = ToEngineVec3(normal);
			outNormal = Vector3(Vector4(n, 0.0f) * transform);
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------
	static bool GetUV(Vector2& outUV, FbxMesh* mesh, int polyIndex, int vertIndex, int uvIndex)
	{
		FbxVector2 uv;
		FbxGeometryElementUV* uvs = mesh->GetElementUV(uvIndex);
		if (GetObjectFromElement(mesh, polyIndex, vertIndex, uvs, &uv))
		{
			outUV = Vector2((float)uv.mData[0], (float)uv.mData[1]);
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------
	static bool GetColor(RGBA& outColor, FbxMesh* mesh, int polyIndex, int vertIndex)
	{
		FbxColor color;
		FbxGeometryElementVertexColor* colors = mesh->GetElementVertexColor(0);
		if (GetObjectFromElement(mesh, polyIndex, vertIndex, colors, &color))
		{
			outColor = ToEngineRGBA(color);
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------
	static void ImportVertex(MeshBuilder& builder, const Matrix4x4& transform, FbxMesh* mesh, int polyIndex, int vertIndex)
	{
		Vector3 normal;
		if (GetNormal(normal, transform, mesh, polyIndex, vertIndex))
		{
			builder.SetNormal(normal);

			//Temporary hack to get around not supporting multiple vertex definitions
			Vector3 bitangent = Vector3::UP;
			if (normal == bitangent)
			{
				bitangent = Vector3::FORWARD;
			}
			Vector3 tangent = Vector3::Cross(bitangent, normal);
			bitangent = Vector3::Cross(normal, tangent);

			builder.SetTangent(tangent);
			builder.SetBitangent(bitangent);
		}
		
		RGBA color;
		if (GetColor(color, mesh, polyIndex, vertIndex))
		{
			builder.SetColor(color);
		}

		Vector2 uv;
		if (GetUV(uv, mesh, polyIndex, vertIndex, 0))
		{
			builder.SetUV(uv);
		}

		Vector3 position;
		if (GetPosition(&position, transform, mesh, polyIndex, vertIndex))
		{
			builder.AddVertex(position);
		}
	}

	static Matrix4x4 GetGeometricTransform(FbxMesh* mesh)
	{
		return Matrix4x4::IDENTITY;
	}

	//-----------------------------------------------------------------------------------
	static void ImportMesh(SceneImport* import, FbxMesh* mesh, MatrixStack4x4& matrixStack)
	{
		MeshBuilder builder = MeshBuilder();
		ASSERT_OR_DIE(mesh->IsTriangleMesh(), "Was unable to load the mesh, it's not a triangle mesh!");
		builder.Begin();
		{
			Matrix4x4 geoTransform = GetGeometricTransform(mesh);
			matrixStack.Push(geoTransform);

			Matrix4x4 transform = matrixStack.GetTop();
			int polyCount = mesh->GetPolygonCount();
			for (int polyIndex = 0; polyIndex < polyCount; ++polyIndex)
			{
				int vertCount = mesh->GetPolygonSize(polyIndex);
				ASSERT_OR_DIE(vertCount == 3, "Vertex count was not 3");
				for (int vertIndex = 0; vertIndex < vertCount; ++vertIndex)
				{
					ImportVertex(builder, transform, mesh, polyIndex, vertIndex);
				}
			}
		}
		builder.End();

		FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(0);
		builder.SetMaterialName(material->GetName());

		matrixStack.Pop();

		import->meshes.push_back(builder);
	}

	//-----------------------------------------------------------------------------------
	static void ImportNodeAttribute(SceneImport* import, FbxNodeAttribute* attrib, MatrixStack4x4& matrixStack)
	{
		if (attrib == nullptr)
		{
			return;
		}

		switch (attrib->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			ImportMesh(import, (FbxMesh*)attrib, matrixStack);
			break;

		default:
			break;
		}
	}

	//-----------------------------------------------------------------------------------
	static Matrix4x4 GetNodeTransform(FbxNode* node)
	{
		FbxMatrix fbxLocalMatrix = node->EvaluateLocalTransform();
		return ToEngineMatrix(fbxLocalMatrix);
	}

	//-----------------------------------------------------------------------------------
	static void ImportSceneNode(SceneImport* import, FbxNode* node, MatrixStack4x4& matrixStack)
	{
		if (node == nullptr)
		{
			return;
		}

		Matrix4x4 nodeLocalTransform = GetNodeTransform(node);
		matrixStack.Push(nodeLocalTransform);

		//New stuff! We want to load it!
		int attributeCount = node->GetNodeAttributeCount();
		for (int attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex)
		{
			ImportNodeAttribute(import, node->GetNodeAttributeByIndex(attributeIndex), matrixStack);
		}

		//Import Children
		int childCount = node->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			ImportSceneNode(import, node->GetChild(childIndex), matrixStack);
		}

		matrixStack.Pop();
	}

	//-----------------------------------------------------------------------------------
	static void TriangulateScene(FbxScene* scene)
	{
		FbxGeometryConverter converter(scene->GetFbxManager());
		converter.Triangulate(scene, true); //True if we should replace the node, false if we want to make new nodes
	}

	//-----------------------------------------------------------------------------------
	static Matrix4x4 GetGeometricTransform(FbxNode* node)
	{
		Matrix4x4 returnMatrix = Matrix4x4::IDENTITY;
		if ((node != nullptr) && (node->GetNodeAttribute() != nullptr))
		{
			const FbxVector4 geoTrans = node->GetGeometricTranslation(FbxNode::eSourcePivot);
			const FbxVector4 geoRot = node->GetGeometricRotation(FbxNode::eSourcePivot);
			const FbxVector4 geoScale = node->GetGeometricScaling(FbxNode::eSourcePivot);

			FbxMatrix geoMat;
			geoMat.SetTRS(geoTrans, geoRot, geoScale);

			returnMatrix = ToEngineMatrix(geoMat);
		}

		return returnMatrix;
	}

	//-----------------------------------------------------------------------------------
	static Skeleton* ImportSkeleton(SceneImport* import, MatrixStack4x4& matrixStack, Skeleton* skeleton, int parentJointIndex, FbxSkeleton* fbxSkeleton)
	{
		Skeleton* returnSkeleton = nullptr;
		if (fbxSkeleton->IsSkeletonRoot())
		{
			//THIS IS NEW SKELETON
			returnSkeleton = new Skeleton();
			import->skeletons.push_back(returnSkeleton);
		}
		else
		{
			returnSkeleton = skeleton;
			ASSERT_OR_DIE(returnSkeleton != nullptr, "Return skeleton was null! (This should never happen lol)");
		}

		Matrix4x4 geotransform = GetGeometricTransform(fbxSkeleton->GetNode());
		matrixStack.Push(geotransform);
		Matrix4x4 modelSpace = matrixStack.GetTop();
		returnSkeleton->AddJoint(fbxSkeleton->GetNode()->GetName(), parentJointIndex, modelSpace);
		matrixStack.Pop();

		return returnSkeleton;
	}

	//-----------------------------------------------------------------------------------
	static void ImportSkeletons(SceneImport* import, FbxNode* node, MatrixStack4x4& matrixStack, Skeleton* skeleton, int parentJointIndex)
	{
		if (nullptr == node)
		{
			return;
		}

		Matrix4x4 mat = GetNodeTransform(node);
		matrixStack.Push(mat);

		//Walk the attributes, looking for doot doots.
		int attributeCount = node->GetNodeAttributeCount();
		for (int attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex)
		{
			FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex(attributeIndex);
			if ((attribute != nullptr) && (attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton))
			{
				//So we have a skeleton
				FbxSkeleton* fbxSkele = (FbxSkeleton*)attribute;
				Skeleton* newSkeleton = ImportSkeleton(import, matrixStack, skeleton, parentJointIndex, fbxSkele);

				//newSkeleton will either be the same skeleton passed, or a new skeleton, or no skeleton if it was a bad node.
				//If we got something back- it's what we p[ass on to the next generation.
				if (newSkeleton != nullptr)
				{
					skeleton = newSkeleton;
					parentJointIndex = skeleton->GetLastAddedJointIndex();
				}
			}
		}

		//do the rest of the tree
		int childCount = node->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			ImportSkeletons(import, node->GetChild(childIndex), matrixStack, skeleton, parentJointIndex);
		}
		matrixStack.Pop();
	}

	//-----------------------------------------------------------------------------------
	static void ImportScene(SceneImport* import, FbxScene* scene, MatrixStack4x4& matrixStack)
	{
		TriangulateScene(scene);
		FbxNode* root = scene->GetRootNode();
		ImportSkeletons(import, root, matrixStack, nullptr, -1);
		ImportSceneNode(import, root, matrixStack);
	}

	//-----------------------------------------------------------------------------------
	static Matrix4x4 GetSceneBasis(FbxScene* scene)
	{
		FbxAxisSystem axisSystem = scene->GetGlobalSettings().GetAxisSystem();
		FbxAMatrix mat;
		axisSystem.GetMatrix(mat);
		return ToEngineMatrix(mat);
	}

	//-----------------------------------------------------------------------------------
	SceneImport* FbxLoadSceneFromFile(const char* fbxFilename, const Matrix4x4& engineBasis, bool isEngineBasisRightHanded, const Matrix4x4& transform)
	{
		FbxScene* scene = nullptr;
		FbxManager* fbxManager = FbxManager::Create();
		if (nullptr == fbxManager)
		{
			Console::instance->PrintLine("Could not create fbx manager.");
			DebuggerPrintf("Could not create fbx manager.");
			return nullptr;
		}

		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT); //Name of object is blank, we don't care
		fbxManager->SetIOSettings(ioSettings);

		//Create an importer
		FbxImporter* importer = FbxImporter::Create(fbxManager, "");

		bool loadSuccessful = importer->Initialize(fbxFilename, -1, fbxManager->GetIOSettings());
		if (loadSuccessful)
		{
			//We have imported the FBX
			scene = FbxScene::Create(fbxManager, "");
			bool importSuccessful = importer->Import(scene);
			ASSERT_OR_DIE(importSuccessful, "Scene import failed!");
		}
		else
		{
			Console::instance->PrintLine(Stringf("Could not import scene: %s", fbxFilename));
			DebuggerPrintf("Could not import scene: %s", fbxFilename);
		}

		SceneImport* import = new SceneImport();
		MatrixStack4x4 matrixStack;

		matrixStack.Push(transform);

		//Set up our initial transforms
		Matrix4x4 sceneBasis = GetSceneBasis(scene);
		Matrix4x4::MatrixTranspose(&sceneBasis);

		if (!isEngineBasisRightHanded)
		{
			Vector3 forward = Matrix4x4::MatrixGetForward(&sceneBasis);
			Matrix4x4::MatrixSetForward(&sceneBasis, -forward); //3rd row or column
		}

		matrixStack.Push(sceneBasis);

		ImportScene(import, scene, matrixStack);

		FBX_SAFE_DESTROY(importer);
		FBX_SAFE_DESTROY(ioSettings);
		FBX_SAFE_DESTROY(scene);
		FBX_SAFE_DESTROY(fbxManager);

		return import;
	}


#else

	void FbxListScene(const char*) {};
	SceneImport* FbxLoadSceneFromFile(const char* fbxFilename, const Matrix4x4& engineBasis, bool isEngineBasisRightHanded, const Matrix4x4& transform) { return nullptr; };

#endif
