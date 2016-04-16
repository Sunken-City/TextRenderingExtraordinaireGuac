#pragma once
#include "Game/Generators/Generator.hpp"
#include "Game/Map/Cell.hpp"
#include <vector>

class DungeonGenerator : public Generator
{
public:
	DungeonGenerator(const std::string& name);
	~DungeonGenerator();

	static Generator* CreateGenerator(const std::string& name) { return new DungeonGenerator(name); }

	//Initializes the default state of the map
	virtual void InitializeMap(Map* map) override;
	//Returns false if there are more steps needed, returns true if that was the last step.
	virtual bool GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map = nullptr) override;

private:
	Vector2Int GetRandomDirection();

	Vector2Int m_currentRootCellLocation;
	static GeneratorRegistration s_dungeonGeneratorRegistration;
};

