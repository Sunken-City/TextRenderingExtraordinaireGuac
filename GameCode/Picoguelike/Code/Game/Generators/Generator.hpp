#pragma once
#include "Engine/Math/Vector2Int.hpp"
#include "Game/Map/Map.hpp"
#include <string>
#include <utility>
#include <map>
#include <string>

//TYPEDEFS//////////////////////////////////////////////////////////////////////////
class Generator;
class GeneratorRegistration;
class GenerationProcessData;
typedef std::pair<std::string, GeneratorRegistration*> GeneratorRegistrationMapPair;
typedef std::map<std::string, GeneratorRegistration*> GeneratorRegistrationMap;
typedef Generator* (GeneratorCreationFunction)(const std::string& name);
typedef GenerationProcessData* (GenerationProcessCreationFunc)(const XMLNode& node);

//-----------------------------------------------------------------------------------
class GenerationProcessData
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	GenerationProcessData(const XMLNode& node);

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	std::string m_generatorName;
	Vector2Int m_rangeSteps;
};

//-----------------------------------------------------------------------------------
class GeneratorRegistration
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	GeneratorRegistration(const std::string& name, GeneratorCreationFunction* creationFunction, GenerationProcessCreationFunc* dataCreationFunction);

	//STATIC FUNCTIONS//////////////////////////////////////////////////////////////////////////
	static Generator* CreateGeneratorByName(const std::string& name);

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	static GeneratorRegistrationMap* s_generatorRegistrationMap;
	
	std::string m_name;
	GenerationProcessCreationFunc* m_dataCreationFunction;
	GeneratorCreationFunction* m_creationFunction;
};

//-----------------------------------------------------------------------------------
class Generator
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	Generator();
	~Generator();

	//VIRTUAL FUNCTIONS//////////////////////////////////////////////////////////////////////////
	//Initializes the default state of the map
	virtual void InitializeMap(Map* map);
	//Returns false if there are more steps needed, returns true if that was the last step.
	virtual bool GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map = nullptr) = 0;

	//STATIC FUNCTIONS//////////////////////////////////////////////////////////////////////////
	static inline GenerationProcessData* CreateGenerationProcessData(const XMLNode& node) { return new GenerationProcessData(node); };
	static void FinalizeMap(Map* map);

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	std::string m_name;
};