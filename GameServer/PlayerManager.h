#pragma once

class Player;

class PlayerShard
{
public:
	void Add(int32 id, PlayerRef player);
	void Remove(int32 id);
	void BroadCast(SendBufferRef sendBuffer);
	PlayerRef GetPlayer(int32 id);
	Map<int32, PlayerRef> GetPlayers() { WRITE_LOCK;  return _players; }
	/* Reuse ID Function */
	int32 ReuseID();
	void PushID(int32 id);

private:
	USE_LOCK;
	Map<int32, PlayerRef> _players;
	PriorityQueue<int32, vector<int32>, greater<int32>> _listOfReuseID;
};

class PlayerManager
{
public:
	enum : int32
	{
		PLAYER_SHARD_COUNT = 4
	};
public:
	PlayerManager();
	~PlayerManager() {}
	PlayerShard* GetShard(int32 playerid);
	void Add(int32 id, PlayerRef player);
	void Remove(int32 id);
	int32 ReuseID(int32 id);
	void BroadCast(SendBufferRef sendBuffer);
	PlayerRef GetPlayer(int32 id);
	//Map<int32, PlayerRef>& GetPlayers()
	/* Reuse ID Function */

private:
	array<unique_ptr<PlayerShard>, PLAYER_SHARD_COUNT> _playerShards;
};

extern PlayerManager GPlayerManager;