#pragma once
#include "Game/Generators/Generator.hpp"
#include "Game/Map/Cell.hpp"
#include <vector>

class FromDataGenerationProcessData : public GenerationProcessData
{
public:
	FromDataGenerationProcessData(const XMLNode& node);

	std::string m_filePath;
};

class FromDataGenerator : public Generator
{
public:
	FromDataGenerator(const std::string& name);
	~FromDataGenerator();

	static Generator* CreateGenerator(const std::string& name) { return new FromDataGenerator(name); }

	//Initializes the default state of the map
	virtual void InitializeMap(Map* map);
	//Returns false if there are more steps needed, returns true if that was the last step.
	virtual bool GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map = nullptr);
	static inline GenerationProcessData* CreateGenerationProcessData(const XMLNode& node) { return new FromDataGenerationProcessData(node); };

private:

	static GeneratorRegistration s_fromDataGeneratorRegistration;
};

