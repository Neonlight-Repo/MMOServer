#include "pch.h"
#include "Player.hpp"
#include "Storage/GameMap.hpp"
#include "Object.hpp"

#include "format"

#include "Manager/DataManager.hpp"

Player::Player(uint64 id, uint32 level, uint32 exp) : GameObject(id, mmo::PLAYER), m_level(level), m_curExp(exp)
{
	SetPosition(Vector2DF::Zero());
	m_airborne = GManager->Data()->GetSkillData(mmo::ESkillType::Airborne);

	UpdateStat();
}

void Player::BeginPlay()
{
	NetObject::BeginPlay();
	m_lastPosX = m_position.x;
	m_isLookAtRight = true;
}

void Player::Tick()
{
	NetObject::Tick();
	auto&& diff = m_position.x - m_lastPosX;
	if (diff > 0)
	{
		m_isLookAtRight = true;
	}
	else if (diff < 0)
	{
		m_isLookAtRight = false;
	}
	m_lastPosX = m_position.x;
}

void Player::OnDestroy(const std::shared_ptr<GameObject>& object)
{
	NetObject::OnDestroy(object);
	if (auto map = GetMap())
	{
		map->Leave(shared_from_this());
	}
}

void Player::OnDamaged(const std::shared_ptr<GameObject> hitter)
{
	m_hitter = hitter;
}

void Player::SetSession(GameSession* session)
{
	m_session = session;
}

void Player::EnterMap(std::shared_ptr<GameMap> gameMap)
{
	if (auto map = m_map.lock())
		LeaveMap();
	
	m_map = gameMap;
	if (auto map = m_map.lock())
		gameMap->Enter(SharedThis());
}

void Player::LeaveMap()
{
	if (auto map = m_map.lock())
	{
		map->Run(&GameMap::Leave, shared_from_this());
		m_map.reset();
	}
}

void Player::TryDamage(const std::shared_ptr<GameObject> hitter)
{
	OnDamaged(hitter);
}

void Player::ObtainItem(Vector<ItemData> items)
{
	for (const auto& item : items)
	{
		m_inventory.items[item.type]++;
	}
}

void Player::AddExp(uint32 exp)
{
	m_curExp += exp;

	auto pstmt = GDatabase->CallProcedure("SP_AddExp", ToAnsiString(m_nickname), exp);
	pstmt->AsyncExecute();

	const auto& requireExp = GManager->Data()->GetRequireExp(m_level + 1);

	/*GManager->Database()->CallProcedure(TEXT("SP_AddExp"), m_nickname, exp);

	const auto& requireExp = GManager->Data()->GetRequireExp(m_level + 1);
	if (requireExp != 0 && m_curExp >= requireExp)
	{
		m_curExp -= requireExp;
		m_level++;
		UpdateStat();
		GManager->Database()->CallProcedure(TEXT("SP_LevelUp"), m_nickname, m_curExp);
	}*/
}

void Player::Airborne() const
{
	auto map = GetMap();
	if (!map)
		return;

	for (const auto& [_, monster] : map->GetMonsters())
	{
		auto diff = monster->GetPosition() - GetPosition();
		if (!m_isLookAtRight)
			diff.x = -diff.x;
		if (diff.x <= m_airborne.range.x && Math::Abs(diff.y) <= m_airborne.range.y)
		{
			monster->Faint(m_airborne.damage);
		}
	}
}

std::shared_ptr<Player> Player::SharedThis()
{
	return std::static_pointer_cast<Player>(shared_from_this());
}

void Player::UpdateStat()
{
	const auto& info = GManager->Data()->GetStatData(m_level);
	SetHp(info.health);
	SetPower(info.power);
}

void Player::SetNickname(StringView nickname)
{
	m_nickname = nickname;
}

GameSession* Player::GetSession()
{
	return m_session;
}

std::shared_ptr<GameMap> Player::GetMap() const
{
	return m_map.lock();
}

String Player::GetNickname() const
{
	return m_nickname;
}

std::shared_ptr<GameObject> Player::GetHitter()
{
	return m_hitter.lock();
}
