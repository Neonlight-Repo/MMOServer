#include "pch.h"
#include "Slime.hpp"
#include "Storage/GameMap.hpp"

Slime::Slime(uint64 id, std::shared_ptr<GameMap> map) : Monster(id, map)
{
	
}

void Slime::BeginPlay()
{
	EnablePatrol(false);
}

void Slime::Tick()
{
	
}