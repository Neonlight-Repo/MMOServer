#include "pch.h"
#include "GameMap.hpp"

#include "Object/Object.hpp"

#include "Session/GameSession.hpp"
#include "Manager/MapManager.hpp"

#include "Manager/ObjectManager.hpp"

GameMap::GameMap(StringView path) : MapData(path)
{
}

GameMap::~GameMap()
{	
}

void GameMap::Broadcast(std::span<char> buffer, uint64 ignore)
{
	for (const auto&[id, player] : m_players)
	{
		if (id == ignore) continue;
		if (auto session = player->GetSession())
			session->SendAtomic(buffer);
	}
}

void GameMap::Broadcast(Packet* packet, uint64 ignore)
{
	for (const auto&[id, player] : m_players)
	{
		if (id == ignore) continue;
		if (auto session = player->GetSession())
			session->Send(packet);
	}
}

void GameMap::Enter(std::shared_ptr<GameObject> object)
{
	auto type = object->GetType();
	if (type == mmo::PLAYER)
	{
		m_players.insert({
			object->GetId(),
			std::static_pointer_cast<Player>(object)
		});
	}
	else if (type >= mmo::MONSTER && type < mmo::THING)
	{
		m_monsters.insert({
			object->GetId(),
			std::static_pointer_cast<Monster>(object)
		});
	}
}

void GameMap::Leave(std::shared_ptr<GameObject> object)
{
	mmo::NotifyLeaveMap leaveMap;
	leaveMap.objectId = object->GetId();

	auto type = object->GetType();
	if (type == mmo::PLAYER)
	{
		m_players.erase(object->GetId());
	}
	else if (type >= mmo::MONSTER && type < mmo::THING)
	{
		m_monsters.erase(object->GetId());
	}
	Broadcast(&leaveMap);
}

void GameMap::SpawnMonster()
{
	auto spawnMonster = GetSpawnMonster();
	if (!spawnMonster.has_value())
		return;

	Run(SpawnTick, &GameMap::SpawnMonster);

	const auto spawnArea = GetBlocks(Block::SpawnArea);
	auto count = m_monsters.size();
	if (spawnArea.size() > 0 && count < 10)
	{
		Vector<mmo::ObjectInfo> infos;
		for (int i = 0; i < 10 - count; ++i)
		{
			auto idx = Random::Range<size_t>(0llu, spawnArea.size() - 1);
			Vector2D spawnPosition = spawnArea[idx];

			std::shared_ptr<Monster> monster = nullptr;
			switch (spawnMonster.value())
			{
			case mmo::Slime:
				monster = GManager->Object()->Create<Slime>(this);
				break;
			case mmo::TurretPlant:
				monster = GManager->Object()->Create<TurretPlant>(this);
				break;
			case mmo::GeminiRobot:
				monster = GManager->Object()->Create<GeminiRobot>(this);
				break;
			default:
				break;
			}
			if (!monster)
				return;

			monster->SetPosition(Vector2DF {
				static_cast<float>(spawnPosition.x),
				static_cast<float>(spawnPosition.y)
			});
			monster->BeginPlay();

			m_monsters[monster->GetId()] = monster;

			mmo::ObjectInfo info;
			info.objectId = monster->GetId();
			info.position = Converter::MakeVector(spawnPosition);
			info.type = spawnMonster.value();
			infos.push_back(info);
		}
		mmo::NotifySpawn spawn;
		spawn.objects = infos;
		Broadcast(&spawn);
	}
}

const HashMap<uint64, std::shared_ptr<Player>>& GameMap::GetPlayers() const
{
	return m_players;
}

const HashMap<uint64, std::shared_ptr<Monster>>& GameMap::GetMonsters() const
{
	return m_monsters;
}

void GameMap::HandleMove(Session* session, std::shared_ptr<gen::mmo::Move> move)
{
	if (!session) return;

	auto gameSession = static_cast<GameSession*>(session);
	auto player = gameSession->GetPlayer();

	gen::mmo::NotifyMove syncMove;
	syncMove.objectId = player->GetId();
	auto prevPos = player->GetPosition();
	auto block = GetBlock(Vector2DI{
		static_cast<int32>(std::round(move->position.x)),
		static_cast<int32>(std::round(move->position.y))
	});
	if (!block.has_value())
	{
		syncMove.position = Converter::MakeVector(prevPos);
		Broadcast(&syncMove);
	}
	else
	{
		switch (block.value())
		{
		case Block::Portal:
			Console::Debug(Category::Temp, TEXT("PORTAL"));
			break;
		case Block::Border:
			syncMove.position = Converter::MakeVector(prevPos);
			player->SetPosition(Converter::MakeVector<float>(syncMove.position));
			Broadcast(&syncMove);
			break;
		default:
			syncMove.position = move->position;
			player->SetPosition(Converter::MakeVector<float>(syncMove.position));
			Broadcast(&syncMove);
			break;
		}
	}
}

void GameMap::HandleLocalChat(Session* session, std::shared_ptr<gen::mmo::Chat> chat)
{
	if (!session) return;

	auto gameSession = static_cast<GameSession*>(session);
	auto sender = gameSession->GetPlayer();

	gen::mmo::NotifyChat notifyChat;
	notifyChat.type = mmo::EChatType::Local;
	notifyChat.senderName = sender->GetNickname();
	notifyChat.message = chat->message;

	Broadcast(&notifyChat, sender->GetId());
}

void GameMap::HandleDamage(Session* session, std::shared_ptr<gen::mmo::AddDamageReq> damage)
{
	const auto& object = GManager->Object()->GetObjectById(damage->damageInfo.objectId);
	if (object)
	{
		mmo::NotifyDamaged notify;
		object->TakeDamage(object, damage->damageInfo.damage);
		notify.damageResult.objectId = object->GetId();
		notify.damageResult.damage = object->GetHp();

		Broadcast(&notify);
	}
}

void GameMap::HandleHitStatus(Session* session, std::shared_ptr<gen::mmo::HitStatus> hit)
{
	if (!session) return;

	const auto& gameSession = static_cast<GameSession*>(session);
	const auto& player = gameSession->GetPlayer();
	const auto& hitter = player->GetHitter();

	if (!player || !hitter)
		return;

	switch (hit->state)
	{
	case mmo::EPlayerState::Parrying:
		break;
	default:
		player->TakeDamage(nullptr, player->GetHitter()->GetPower()); // To reset player's hitter
		break;
	}
}

void GameMap::HandleSkillActivate(Session* session, std::shared_ptr<gen::mmo::SkillActivate> skill)
{
	if (!session) return;

	auto gameSession = static_cast<GameSession*>(session);
	if (auto player = gameSession->GetPlayer())
	{
		switch (skill->type)
		{
		case mmo::Airborne:
			player->Airborne();
			break;
		default:
			break;
		}
	}
}

void GameMap::BeginPlay()
{
	if (auto spawnMonster = GetSpawnMonster(); spawnMonster.has_value())
		SpawnMonster();
}

void GameMap::Tick()
{
	Run(GameTick, &GameMap::Tick);

	// Execute NetObject's `Tick` logic
	for (const auto& [_, object] : m_players)
	{
		object->Tick();
	}
	for (const auto&[_, object] : m_monsters)
	{
		object->Tick();
	}
}
