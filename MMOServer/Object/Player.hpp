#pragma once

#include "GameObject.hpp"
#include "Storage/Datasheet.hpp"
#include "generated/mmo/Protocol.gen.hpp"

class GameSession;
class GameMap;

struct Inventory
{
	Inventory() = default;
	ConcurrencyHashMap<mmo::EItemType, uint32> items;
};

class Player : public GameObject
{
	USE_POOL(Player)
public:
	Player(uint64 id, uint32 level, uint32 exp);
public:
	/* Events */
	virtual void BeginPlay() override;
	virtual void Tick() override;
	virtual void OnDestroy(const std::shared_ptr<GameObject>&) override;
	virtual void OnDamaged(const std::shared_ptr<GameObject>) override;
public:
	/* Get / Set */
	void SetSession(GameSession* session);
	void SetNickname(StringView nickname);
	GameSession* GetSession();
	std::shared_ptr<GameMap> GetMap() const;
	String GetNickname() const;
	std::shared_ptr<GameObject> GetHitter();
public: 
	/* Interactions */
	void EnterMap(std::shared_ptr<GameMap> gameMap);
	void LeaveMap();

	void TryDamage(const std::shared_ptr<GameObject> hitter);

	void ObtainItem(Vector<ItemData> item);
	void AddExp(uint32 exp);
public:
	/* Skills */
	void Airborne() const;
private:
	std::shared_ptr<Player> SharedThis();
	void UpdateStat();
private:
	Inventory m_inventory;
	uint16 m_level;
	uint32 m_curExp;

	GameSession* m_session;
	std::weak_ptr<GameMap> m_map;
	std::weak_ptr<GameObject> m_hitter;
	String m_nickname;
	
	int m_lastPosX;
	bool m_isLookAtRight;

	SkillData m_airborne;
};