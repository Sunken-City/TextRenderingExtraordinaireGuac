#pragma once
#include "Game/Generators/Generator.hpp"
#include "Game/Map/Cell.hpp"
#include <vector>

class HiveGenerator : public Generator
{
public:
	HiveGenerator(const std::string& name);
	~HiveGenerator();

	static Generator* CreateGenerator(const std::string& name) { return new HiveGenerator(name); }

	//Initializes the default state of the map
	virtual void InitializeMap(Map* map) override;
	//Returns false if there are more steps needed, returns true if that was the last step.
	virtual bool GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map = nullptr) override;

private:

	void SpawnFeelersAt(const Vector2Int& location);

	struct Feeler
	{
		enum FeelerState
		{
			UP,
			UP2,
			CENTER,
			DOWN,
			DOWN2,
			CENTER2,
			NUM_STATES
		};

		Feeler(Vector2Int position, Vector2Int direction, FeelerState state) : currentPosition(position), currentDirection(direction), isFinished(false), stateNumber(state) {};
		bool TakeStep(Map* map, GenerationProcessData* data, HiveGenerator* generator);
		void IncrementState(HiveGenerator* generator);

		Vector2Int currentPosition;
		Vector2Int currentDirection;
		bool isFinished;
		FeelerState stateNumber;
	};

	std::vector<Feeler>* m_currentFeelers;
	std::vector<Feeler>* m_nextFeelers;
	static GeneratorRegistration s_hiveGeneratorRegistration;
};

