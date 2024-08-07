#pragma once
#include "Functor.hpp"
#include "Storage/MapData.hpp"

#include "generated/mmo/ServerPacketHandler.gen.hpp"

class GameObject;

class GameMap :
	public Runnable<GameMap>,
	public MapData
{
	enum
	{
		GameTick = 33, // for 30fps tick rate
		SpawnTick = 12000
	};	
public:
	GameMap(StringView path);
	~GameMap();
public:
	void Broadcast(std::span<char> buffer, uint64 ignore = 0);
	void Broadcast(Packet* packet, uint64 ignore = 0);
	void Enter(std::shared_ptr<GameObject> object);
	void Leave(std::shared_ptr<GameObject> object);
	void SpawnMonster();
public:
	const HashMap<uint64, std::shared_ptr<class Player>>& GetPlayers() const;
	const HashMap<uint64, std::shared_ptr<class Monster>>& GetMonsters() const;
public:
	void HandleMove(Session* session, std::shared_ptr<gen::mmo::Move> move);
	void HandleLocalChat(Session* session, std::shared_ptr<gen::mmo::Chat> chat);
	void HandleDamage(Session* session, std::shared_ptr<gen::mmo::AddDamageReq> damage);
	void HandleHitStatus(Session* session, std::shared_ptr<gen::mmo::HitStatus> hit);
	void HandleSkillActivate(Session* session, std::shared_ptr<gen::mmo::SkillActivate> skill);
public:
	void BeginPlay();
	void Tick();
private:
	HashMap<uint64, std::shared_ptr<class Player>> m_players;
	HashMap<uint64, std::shared_ptr<class Monster>> m_monsters;
};

