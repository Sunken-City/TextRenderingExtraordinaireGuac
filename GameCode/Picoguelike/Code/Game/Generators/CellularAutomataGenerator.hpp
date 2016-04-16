#pragma once
#include "Game/Generators/Generator.hpp"
#include "Game/Map/Cell.hpp"
#include <vector>

class CellularAutomataGenerator : public Generator
{
public:
	CellularAutomataGenerator(const std::string& name);
	~CellularAutomataGenerator();

	static Generator* CreateGenerator(const std::string& name) { return new CellularAutomataGenerator(name); }

	//INHERITED METHODS//////////////////////////////////////////////////////////////////////////
	//Initializes the default state of the map
	virtual void InitializeMap(Map* map);
	//Returns false if there are more steps needed, returns true if that was the last step.
	virtual bool GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map = nullptr) override;

	//HELPER FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void StoreInitialMapState(Map* map);

private:
	std::vector<Cell::Type> m_nextState;

	static GeneratorRegistration s_cellularAutomatonGeneratorRegistration;
};