#include "pch.h"
#include "MMOServer.hpp"
#include "Manager/Manager.hpp"
#include "Database/DBConnectionPool.hpp"
#include "Session/GameSession.hpp"
#include "Session/LogSession.hpp"
#include "Network/Server.hpp"
#include "Network/Client.hpp"

Engine* GEngine;
Manager* GManager;

MMOServer::MMOServer()
{
	GEngine = new Engine;
	GManager = new Manager;
}

MMOServer::~MMOServer()
{
	delete GEngine;
	delete GManager;
}

void MMOServer::Run()
{
	GEngine->Initialize();
	GManager->Initialize();

	auto logEndpoint = Endpoint(net::IpAddress::Loopback, 1225);
	auto serverEndpoint = Endpoint(IpAddress::Loopback, 1004);
	auto server = Server::Open<GameSession>();
	auto client = Client::Open<LogSession>();
	server->Run(serverEndpoint);
	client->Run(logEndpoint);

	Console::Log(Category::MMOServer, TEXT("MMO Server is running on ") + ToUnicodeString(serverEndpoint.toString()));

	GEngine->ExecuteThread(4, 4);
}
