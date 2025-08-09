#include "pch.h"
#include "PlayerManager.h"
#include "Player.h"
PlayerManager GPlayerManager;



PlayerManager::PlayerManager()
{
	for (int i = 0; i < PLAYER_SHARD_COUNT; i++)
	{
		_playerShards[i] = make_unique<PlayerShard>();
	}
}
PlayerShard* PlayerManager::GetShard(int32 playerid)
{
	return _playerShards[playerid % PLAYER_SHARD_COUNT].get();
}
void PlayerManager::Add(int32 id, PlayerRef player)
{
	GetShard(id)->Add(id, player);
	//_players.insert(make_pair(id, player));
}

void PlayerManager::Remove(int32 id)
{
	GetShard(id)->Remove(id);
}

void PlayerManager::BroadCast(SendBufferRef sendBuffer)
{
	for (int i = 0; i < PLAYER_SHARD_COUNT; i++)
	{
		GetShard(i)->BroadCast(sendBuffer);
	}
}

PlayerRef PlayerManager::GetPlayer(int32 id)
{
	return GetShard(id)->GetPlayer(id);
}

void PlayerShard::Add(int32 id, PlayerRef player)
{
	WRITE_LOCK;
	_players[id] = player;
	//_players.insert(make_pair(id, player));
}

void PlayerShard::Remove(int32 id)
{
	WRITE_LOCK;
	_players.erase(id);
}

void PlayerShard::BroadCast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (auto player : _players)
	{
		player.second->Send(sendBuffer);
	}
}

PlayerRef PlayerShard::GetPlayer(int32 id)
{
	WRITE_LOCK;

	auto player = _players.find(id);
	if (player == _players.end()) // 존재하지않음
		return nullptr;

	return (player->second);
}


int32 PlayerShard::ReuseID()
{
	WRITE_LOCK;

	if (_listOfReuseID.empty())
		return -1;

	int32 id = _listOfReuseID.top();
	_listOfReuseID.pop();

	return id;
}

void PlayerShard::PushID(int32 id)
{
	WRITE_LOCK;

	_listOfReuseID.push(id);
}