#include "pch.h"
#include "GameMap.hpp"
#include "Player.hpp"
#include "Session/GameSession.hpp"
#include "Manager/MapManager.hpp"

GameMap::GameMap()
{
}

GameMap::GameMap(StringView path) : MapData(path)
{
}

GameMap::~GameMap()
{
}

void GameMap::Broadcast(Packet* packet, uint64 ignore)
{
	for (const auto& player : m_players)
	{
		if (player.first != ignore)
			player.second->GetSession()->Send(packet);
	}
}

Vector<std::shared_ptr<Player>> GameMap::Players()
{
	Vector<std::shared_ptr<Player>> players;
	for (const auto& player : m_players)
	{
		players.push_back(player.second);
	}
	return players;
}

void GameMap::AddPlayer(std::shared_ptr<class Player> player)
{
	m_players.insert({ player->GetId(), player });
}

void GameMap::RemovePlayer(std::shared_ptr<class Player> player)
{
	m_players.erase(player->GetId());
}

void GameMap::HandleMove(std::shared_ptr<Session> session, gen::mmo::Move move)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	gen::mmo::NotifyMove syncMove;
	syncMove.position = Converter::MakeVector(gameSession->GetPlayer()->GetPosition() + Converter::MakeVector<int>(move.dir));
}

void GameMap::HandleLocalChat(std::shared_ptr<Session> session, gen::mmo::Chat chat)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	auto sender = gameSession->GetPlayer();

	gen::mmo::NotifyChat notifyChat;
	notifyChat.senderName = sender->GetNickname();
	notifyChat.message = chat.message;

	Broadcast(&notifyChat, sender->GetId());
}

void GameMap::HandleTeleport(std::shared_ptr<Session> session, gen::mmo::TeleportReq teleport)
{
	auto player = m_players[teleport.objectId];
	for (auto portal : m_portals)
	{
		if (portal.position == player->GetPosition())
			player->EnterMap(GManager->Map()->GetMap(portal.destMap));
	}
}