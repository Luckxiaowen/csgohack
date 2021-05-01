#pragma once
#include "csgo.h"
#include "offset.h"
#include <iostream>
using namespace std;
void glow()
{

		DWORD glowObject = ReadGameMemory<DWORD>(client + offset.GlowObjectManager);
		DWORD localPlayer = getLocalPlayer();
		if (localPlayer == NULL)
		{
			Sleep(3000);
			return;
		}
		int myTeam = getTame(localPlayer);
		for (int i = 0; i < 64; i++)
		{
			DWORD entity = getPlayer(i);
			if (entity != NULL)
			{
				int glowIndex = ReadGameMemory<int>(entity + offset.GlowIndex);
				int entityTeam = getTame(entity);
				if (entityTeam == myTeam)
				{
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0x4, 0);
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0x8, 0);
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0xC, 2);
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0x10, 1.7);
				}
				else
				{
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0x4, 2);
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0x8, 0);
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0xC, 0);
					WriteGameMemory<float>(glowObject + (glowIndex * 0x38) + 0x10, 1.7);
				}
				WriteGameMemory<bool>(glowObject + (glowIndex * 0x38) + 0x24, true);
				WriteGameMemory<bool>(glowObject + (glowIndex * 0x38) + 0x25, false);
			}
		}

	
}


int WINAPI MainThred(HMODULE hModule)
{
	int isShoot = false;
	
		
		while (1)
		{
			if (GetAsyncKeyState(VK_F2) & 1)
			{
				isShoot = !isShoot;
				printf("自动开枪:%d\n", isShoot);
			}
			if (isShoot)
			{
				int crosshairId = getCrosshairId();
				int entilyTeam = getTame(getPlayer(crosshairId - 1));
				int myTeam = getTame(getLocalPlayer());
				if (crosshairId > 0 && crosshairId < 32 && entilyTeam != myTeam)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, 0, 0);
					//Sleep(200);
				}
				Sleep(50);
			}
			
			
		}
	
	
	return 0;
}

DWORD Aimbot;
void aimBot()
{
	float aimDistance = 9999.0;
	//获取自己信息
	DWORD localPlayer = getLocalPlayer();

	if (localPlayer == NULL)return;
	int myTeam = getTame(localPlayer);
	D3DXVECTOR3 localbone1 = get_bone_pos(localPlayer,8);
	vec3 localbone = GetBonePos(localPlayer);

	D3DXVECTOR3 vest;
	vec3 recoil_angle = get_recoil_angle();//后坐力
	vec3 current_angle = get_angle();
	bool state = false;
	D3DXVECTOR3 bone1;
	D3DXVECTOR3 local_pos;
	for (int i = 0; i < 32; i++)
	{
		DWORD entily = getPlayer(i);
		if (get_immunity(entily))continue;
		if (cheak(entily, localPlayer))
		{
			ReadProcessMemory(hProcess,(LPCVOID) (client + offset.ViewMatrix), &matrix, sizeof(matrix), 0);
			DWORD boneAdder = getBoneAdder(entily);
			if (boneAdder == NULL)continue;

			bone1 = get_bone_pos(entily, 8);

			
			vec3 bone = GetBonePos(entily);
			
			float dis = getDistance(localbone, bone);
			if (dis >= 3000.f)continue;
			vec2 bone2d; 
			WorldToScreen(bone, bone2d);


			/* 获取自己眼睛位置 */
			local_pos = ReadGameMemory<D3DXVECTOR3>(localPlayer + offset.vecOrigin);
			local_pos += ReadGameMemory<D3DXVECTOR3>(localPlayer +offset.m_vecViewOffset);


			

			/* 判断是否可见 */
			//if (m_bsp.is_visible(local_pos, bone1) == false) continue;

			float f = getCrossHairDistance(bone2d.x,bone2d.y);
			if (f < aimDistance)
			{
				aimDistance = f;
				vest = bone1;
				state = true;
			}			
		}
	}
	if (state)
	{

	

		/* 设置角度 */


		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{


			/* 计算自瞄角度 */
			D3DXVECTOR3 angle = get_aimbot_angle(local_pos, vest);

			/* 获取后坐力 */
			D3DXVECTOR3 punch = ReadGameMemory<D3DXVECTOR3>(localPlayer + offset.aimPunchAngle) * 2;
			if (abs(punch.x) > 1.0f) return;

			angle -= punch;
			/* 归一化角度 */
			angle_normalize(angle);
			//clamp_angles(angle);





			set_angle(angle);
			//vec2 angle = aimCount(localbone, vest);

			//set_angle(angle);
			//WriteGameMemory<float>(Mouse + 0x4d90, angle.y- (recoil_angle.y*2));
			//WriteGameMemory<float>(Mouse + 0x4d94, angle.x- (recoil_angle.x*2));
		//}

		}

	}
}




/* 人物自瞄 */
bool  aimbot()
{
	/* 获取自己基址 */
	DWORD local_player = getLocalPlayer();
	if (local_player == 0) return false;

	/* 获取自己存活状态 */
	if (get_life_state(local_player) == false) return false;

	/* 设置一下视角 */
	WriteGameMemory<DWORD>(local_player + 0x332c, 120);

	/* 获取当前武器索引 */
	short weapon_index = get_weapon_index(local_player);
	if (IsIDKnife(weapon_index) || IsIDOther(weapon_index)) return false;

	/* 获取自己眼睛位置 */
	D3DXVECTOR3 local_pos = ReadGameMemory<D3DXVECTOR3>(local_player + offset.vecOrigin);
	local_pos += ReadGameMemory<D3DXVECTOR3>(local_player + offset.m_vecViewOffset);

	/* 获取自己阵营 */
	int local_team = getTame(local_player);

	/* 遍历玩家 */
	for (int i = 0; i < 64; i++)
	{
		/* 获取玩家基址 */
		DWORD entity = getPlayer(i);
		if (entity == 0 || entity == local_player) continue;

		/* 判断血量 */
		if (get_life_state(entity) == false) continue;

		/* 获取玩家阵营 */
		if (local_team == getTame(entity)) continue;

		/* 获取玩家买枪状态 */
		if (get_immunity(entity)) continue;

		/* 获取玩家头部骨骼 */
		D3DXVECTOR3 bone = get_bone_pos(entity, 8);

		/* 判断是否可见 */
		if (m_bsp.is_visible(local_pos, bone) == false) continue;

		/* 计算自瞄角度 */
		D3DXVECTOR3 angle = get_aimbot_angle(local_pos, bone);

		/* 获取后坐力 */
		D3DXVECTOR3 punch = ReadGameMemory<D3DXVECTOR3>(local_player + offset.aimPunchAngle) * 2;
		if (abs(punch.x) > 1.0f) continue;
		angle -= punch;

		/* 归一化角度 */
		angle_normalize(angle);
		//clamp_angles(angle);

		/* 设置角度 */
		set_angle(angle);

		/* 是否顶置窗口 */
		bool top = FindWindowA(nullptr, "Counter-Strike: Global Offensive") == GetForegroundWindow();

		/* 开镜 */
		if (top && IsIDSniper(weapon_index))
		{
			if (get_scoped(local_player) == false)
			{
				mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
				Sleep(150);//小狙
				//Sleep(400);//大狙
				return false;
			}
			//else if (get_two_scoped(local_player) == false) mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		}

		/* 开枪 */
		//mouse_left_down();
		mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, 0, 0);
		//Sleep(200);
		return true;
	}

	return false;
}





