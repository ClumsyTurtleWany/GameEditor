#pragma once
#include "System.hpp"
#include "World.hpp"
#include <string>

class SaveLoadSystem : public ECS::System
{
public:
	bool SaveFlag = false;
	bool LoadFlag = false;
	std::string SaveFileName;
	std::string LoadFileName;
	std::string Path;

public:
	virtual void Tick(ECS::World* world, float time) override;

public:
	void Save(std::string path, std::string filename);
	void Load(std::string path, std::string filename);
};