#pragma once
#include "Thread/JobSerializer.hpp"
#include "Object/NetObject.hpp"

#include "generated/mmo/Protocol.gen.hpp"

class NetObjectManager : public JobSerializer
{
public:
	NetObjectManager();
public:
	template<class T>
	std::shared_ptr<T> Create()
	{
		auto object = MakeShared<T>(++m_lastId);
		m_objects.insert({ m_lastId, object });
		return object;
	}
public:
	void HandleEnterGame(std::shared_ptr<class Session> session, gen::mmo::EnterGameReq req);
	void HandleDirectChat(std::shared_ptr<class Session> session, gen::mmo::Chat chat);
	void HandleAllChat(std::shared_ptr<class Session> session, gen::mmo::Chat chat);
private:
	void BroadcastAll(Packet* packet, uint64 ignore = 0);
private:
	std::atomic<uint64> m_lastId;
	ConcurrencyHashMap<uint64, std::shared_ptr<class NetObject>> m_objects;
};

