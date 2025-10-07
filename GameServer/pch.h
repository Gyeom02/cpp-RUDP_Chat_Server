#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define _SERVER
//#define ENCRYPTED_HARD //��� ��Ŷ�� ��ȣȭ�� ������ �Ǵ�
#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "CorePch.h"
#include "Enum.pb.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Encrypt.h"
using GameSessionRef = shared_ptr<class GameSession>;
//using PlayerRef = shared_ptr<class Player>;