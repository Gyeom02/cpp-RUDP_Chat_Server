#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
//#define _SERVER
#ifdef _DEBUG
//#define ENCRYPTED_HARD //모든 패킷을 암호화할 것인지 판단
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif


//#include <Windows.h>
#define _NOCTYPT
#define VC_EXTRALEAN
#include <afxwin.h>       // MFC core and standard components
#include <afxext.h>  
#include "afxbutton.h"

#include "CorePch.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "PlayerManager.h"
#include "Encrypt.h"


//#include <afxwin.h>
