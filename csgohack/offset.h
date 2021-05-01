#pragma once
#include <windows.h>
struct gameOffset
{
	DWORD localplayer = 0xD8D2BC;
	DWORD GlowObjectManager = 0x52ED560;
	DWORD team = 0xF4;
	DWORD EntityList = 0x4DA4F8C;
	DWORD GlowIndex = 0xA438;
	DWORD health = 0x100;
	DWORD Dormant = 0xED;//休眠
	DWORD CrosshairId = 0xB3E4;
	DWORD ViewAngles = 0x4D90;
	DWORD BoneMatrix = 0x26A8;
	DWORD ViewMatrix = 0x4D968A4;
	DWORD vecOrigin = 0x138;
	DWORD aimPunchAngle = 0x302C;
	DWORD GameDir = 0x62D7F8;
	DWORD dwClientState = 0x589FE4;
	DWORD dwClientState_Map = 0x28C;
	DWORD m_vecViewOffset = 0x108;
	DWORD dwForceAttack = 0x31D54DC;
	DWORD dwClientState_ViewAngles = 0x4D90;
	DWORD m_hActiveWeapon = 0x2EF8;
	DWORD m_iItemDefinitionIndex = 0x2FAA;
	DWORD m_bGunGameImmunity = 0x3944;//买枪状态
	DWORD m_lifeState = 0x25F;//存货状态
	DWORD m_bIsScoped = 0x3928;
}offset;