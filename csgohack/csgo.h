#pragma once
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "offset.h"
#include <cmath>
#include "bsp_parser.hpp"
#define pi 3.141592653
enum CSWeaponID_t : uint16_t //enum  ItemSchemaIndex_e
{
	WEAPON_NONE = 0,

	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SHIELD,
	WEAPON_SCAR20,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFEGG,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_SNOWBALL,
	WEAPON_BUMPMINE,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT,
	WEAPON_KNIFE_KARAMBIT,
	WEAPON_KNIFE_M9_BAYONET,
	WEAPON_KNIFE_TACTICAL,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER
};
DWORD gamePid;//游戏pid
HANDLE hProcess;//游戏进程句柄
HWND HGamewnd;//游戏窗口句柄
DWORD client;//客服端模块地址
DWORD Mouse;
DWORD engine;
float matrix[4][4];
RECT rect;
int client_width;
int client_height;
std::string m_path;
rn::bsp_parser m_bsp;



struct vec3
{
	float x, y, z;
};

struct vec2
{
	float x, y;
};

struct vec4
{
	float x, y, z, w;
};
//读内存
template <typename ReadType>
ReadType ReadGameMemory(DWORD addr)
{
	ReadType buff;
	ReadProcessMemory(hProcess, (LPCVOID)addr, &buff, sizeof(ReadType), NULL);
	return buff;
}
//获取模块
DWORD GetprocessModuleBase(const wchar_t *moduleName)
{
	HANDLE hMoudleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	hMoudleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, gamePid);
	if (hMoudleSnap == INVALID_HANDLE_VALUE)
	{
		printf("获取失败 \n");
		return 0;
	}
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hMoudleSnap, &me32))
	{
		printf("获取失败 \n");
		return 0;
	}
	do
	{
		if (!wcscmp(me32.szModule, moduleName))
		{
			CloseHandle(hMoudleSnap);
			return (DWORD)me32.modBaseAddr;
		}
	} while (Module32Next(hMoudleSnap, &me32));
	CloseHandle(hMoudleSnap);
	return 0;
}

template <typename T>
void WriteGameMemory(DWORD addr, T write)
{
	WriteProcessMemory(hProcess, (PVOID)addr, (PVOID)&write, sizeof(T), NULL);

}

/* 读取字符串 */
std::string read_string(DWORD addr, int size)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, gamePid);
	if (hProcess == NULL) return "";

	SIZE_T bytes = 0;
	std::string buffer;
	buffer.resize(size);
	ReadProcessMemory(hProcess, (LPCVOID)addr,(LPVOID)( buffer.data()), size, &bytes);

	CloseHandle(hProcess);

	return buffer;
}

/* 鼠标左键按下 */
inline void mouse_left_down()
{
	WriteGameMemory<bool>(client + offset.dwForceAttack, true);
	Sleep(5);
	WriteGameMemory<bool>(client + offset.dwForceAttack, false);
}

/* 更新地图 */
void update_map()
{
	static std::string last_map = "";
	static bool state = false;

	DWORD addr = ReadGameMemory<DWORD>(engine + offset.dwClientState);
	if (addr)
	{
		std::string temp = read_string(addr + offset.dwClientState_Map, 64);
		std::string current_map = temp.substr(0, strlen(temp.c_str())) + ".bsp";
		if (state == false || last_map != current_map)
		{
			last_map = current_map;
			state = m_bsp.load_map(m_path, current_map);
			std::cout << "[+] 当前游戏地图 : " << last_map << " - " << (state == true ? "成功" : "失败") << std::endl;
			Sleep(300);
		}
	}
}
//初始化游戏数据

void inItGame()
{
	HGamewnd = FindWindow(L"Valve001", L"Counter-Strike: Global Offensive");
	if (HGamewnd == NULL)
	{
		printf("初始化失败!\n");
		printf("请打开游戏进行对局后开启\n");
		system("pause");
		exit(-1);
	}
	GetWindowThreadProcessId(HGamewnd, &gamePid);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, gamePid);
	client = GetprocessModuleBase(L"client.dll");
	engine = GetprocessModuleBase(L"engine.dll");
	Mouse = ReadGameMemory<DWORD>(engine + 0x588FE4);
	GetClientRect(HGamewnd, &rect);
	client_width = (rect.right - rect.left);
	client_height = (rect.bottom - rect.top);
	printf("初始化成功!\n");
	printf("更新时间2021-04-20-18.04\n");
	printf("[+]窗口宽:%d\n", client_width);
	printf("[+]窗口宽:%d\n", client_height);
	printf("[+]进程ID:%d\n", gamePid);
	printf("[+]client基址:%x\n", client);
	printf("[+}engine基址:%x\n", engine);
	printf("[+]视角基址:%x\n", Mouse);
	printf("[+]自动开枪开启成功!\n");
	printf("[+]按住shift进行自瞄\n");
	/*std::string temp = read_string(engine + offset.GameDir, 150);
	m_path = temp.substr(0, strlen(temp.c_str())) + "\\maps";
	std::cout << "[+] 当前游戏路径 : " << m_path << std::endl;
	update_map();*/
}







DWORD getLocalPlayer()
{
	return ReadGameMemory<DWORD>(client + offset.localplayer);
}


DWORD getPlayer(int index)
{
	return ReadGameMemory<DWORD>(client + offset.EntityList + index * 0x10);
}

int getTame(DWORD player)
{
	return ReadGameMemory<int>(player + offset.team);
}

int getHealth(DWORD player)
{
	return ReadGameMemory<int>(player + offset.health);
}

int getDorment(DWORD player)
{
	return ReadGameMemory<int>(player + offset.Dormant);
}


int getCrosshairId()
{
	return ReadGameMemory<int>(getLocalPlayer() + offset.CrosshairId);
}

float getDistance(vec3 local, vec3 entity)
{
	float x = local.x - entity.x;
	float y = local.y - entity.y;
	float z = local.z - entity.z;
	return (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

float getCrossHairDistance(float x,float y)
{
	float x1 = client_width / 2 - x;
	float y1 = client_height /2 - y;

	return (float)sqrt(pow(x1, 2.0) + pow(y1, 2.0));
}

vec2 getMouse()
{
	return ReadGameMemory<vec2>(engine + offset.ViewAngles);
}

DWORD getBoneAdder(DWORD player)
{
	return ReadGameMemory<DWORD>(player + offset.BoneMatrix);
}

BOOL WorldToScreen(vec3 from, vec2 &screen) // 3D to 2D
{
	vec4 clipCoords;
	clipCoords.x = matrix[0][0] * from.x + matrix[0][1] * from.y + matrix[0][2] * from.z + matrix[0][3];
	clipCoords.y = matrix[1][0] * from.x + matrix[1][1] * from.y + matrix[1][2] * from.z + matrix[1][3];
	clipCoords.z = matrix[2][0] * from.x + matrix[2][1] * from.y + matrix[2][2] * from.z + matrix[2][3];
	clipCoords.w = matrix[3][0] * from.x + matrix[3][1] * from.y + matrix[3][2] * from.z + matrix[3][3];

	/*if (clipCoords.w < 0.1f)
		return false;*/

	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (client_width / 2 * NDC.x) + (NDC.x + client_width / 2);
	screen.y = -(client_height / 2 * NDC.y) + (NDC.y + client_height / 2);

	return TRUE;
}

vec3 getPosition(DWORD player)
{
	return ReadGameMemory<vec3>(player + offset.vecOrigin);
}

BOOL cheak(DWORD player,DWORD localplayer)
{
	if (player ==localplayer)return FALSE;
	if (player == NULL)return FALSE;
	if (getTame(player) == getTame(localplayer))return FALSE;
	if (getHealth(player) <= 0 || getHealth(player) > 100)return FALSE;
	if (getDorment(player) == 1)return FALSE;
	return TRUE;
}


vec2 aimCount(vec3 me, vec3 player)
{
	vec2 mouse;
	float x = me.x - player.x;
	float y = me.y - player.y;
	float z = me.z - player.z;
	if (x >= 0 && y >= 0)
		mouse.x = atan(y / x) / pi * 180 - 180;
	if (x <= 0 && y >= 0)
		mouse.x = atan(y / x) / pi * 180;
	if (x <= 0 && y <= 0)
		mouse.x = atan(y / x) / pi * 180;
	if (x >= 0 && y <= 0)
		mouse.x = atan(y/x) / pi * 180 + 180;

	mouse.y = atan(z / sqrt(x*x + y * y)) / pi * 180;
	return mouse;


}







/* 计算自瞄角度 */
D3DXVECTOR3 get_aimbot_angle(D3DXVECTOR3& vecOrigin, D3DXVECTOR3& vecOther)
{
	D3DXVECTOR3 vecAngles{};
	D3DXVECTOR3 vecDelta = D3DXVECTOR3((vecOrigin[0] - vecOther[0]), (vecOrigin[1] - vecOther[1]), (vecOrigin[2] - vecOther[2]));
	float hyp = sqrtf(vecDelta[0] * vecDelta[0] + vecDelta[1] * vecDelta[1]);

	float  M_PI = 3.14159265358979323846f;
	vecAngles[0] = (float)atan(vecDelta[2] / hyp)		*(float)(180.f / M_PI);
	vecAngles[1] = (float)atan(vecDelta[1] / vecDelta[0])	*(float)(180.f / M_PI);
	vecAngles[2] = (float)0.f;

	if (vecDelta[0] >= 0.f) vecAngles[1] += 180.0f;
	return vecAngles;
}


/* 角度归一化 */
void angle_normalize(D3DXVECTOR3& vAngles)
{
	for (int i = 0; i < 3; i++)
	{
		if (vAngles[i] < -180.0f) vAngles[i] += 360.0f;
		if (vAngles[i] > 180.0f) vAngles[i] -= 360.0f;
	}

	if (vAngles.x < -89.0f) vAngles.x = 89.0f;
	if (vAngles.x > 89.0f) vAngles.x = 89.0f;

	vAngles.z = 0;
}

/* 规范角度 */
void clamp_angles(vec3& vAngles)
{
	while (vAngles.y < -180.0f) vAngles.y += 360.0f;
	while (vAngles.y > 180.0f) vAngles.y -= 360.0f;

	if (vAngles.x < -89.0f) vAngles.x = 89.0f;
	if (vAngles.x > 89.0f) vAngles.x = 89.0f;

	vAngles.z = 0;
}

vec3 GetBonePos(int iEntityBase)
{
	int BoneMatrix = ReadGameMemory<int>(iEntityBase + offset.BoneMatrix);
	vec3 bone;
	bone.x = ReadGameMemory<float>(BoneMatrix + 0x18C);
	bone.y = ReadGameMemory<float>(BoneMatrix + 0x19C);
	bone.z = ReadGameMemory<float>(BoneMatrix + 0x17C);

	return bone;
}

/* 获取骨骼位置 */
D3DXVECTOR3 get_bone_pos(DWORD base, int index)
{
	typedef struct {
		float Matrix[3][4];
	} Matrix3x4_t;

	DWORD addr = ReadGameMemory<DWORD>(base + offset.BoneMatrix);
	if (addr)
	{
		Matrix3x4_t result = ReadGameMemory<Matrix3x4_t>(addr + index * 0x30);

		return{
			result.Matrix[0][3],
			result.Matrix[1][3],
			result.Matrix[2][3]
		};
	}

	return {  };
}

/* 获取角度 */
vec3 get_angle()
{
	return ReadGameMemory<vec3>(getLocalPlayer()+offset.aimPunchAngle); //apex_offsets::m_localAngles;
}


void calc_angle(vec3& vecOrigin, vec3& vecOther, vec3& vecAngles)
{
	vec3 vecDelta = vec3{ (vecOrigin.x - vecOther.x), (vecOrigin.y - vecOther.y), (vecOrigin.z - vecOther.z) };
	float hyp = sqrtf(vecDelta.x * vecDelta.x + vecDelta.y * vecDelta.y);

	vecAngles.x = (float)atan(vecDelta.z / hyp)		*(float)(180.f / 3.14159265358979323846);
	vecAngles.y = (float)atan(vecDelta.y / vecDelta.x)	*(float)(180.f / 3.14159265358979323846);
	vecAngles.z = (float)0.f;

	if (vecDelta.x >= 0.f) vecAngles.y += 180.0f;
}


/* 获取当前武器索引 */
short get_weapon_index(DWORD base)
{
	int index = ReadGameMemory<DWORD>(base + offset.m_hActiveWeapon) & 0xfff;
	base = ReadGameMemory<DWORD>(client + offset.EntityList + (index - 1) * 0x10);
	return ReadGameMemory<short>(base + offset.m_iItemDefinitionIndex);
}
/* 获取买枪状态 */
bool get_immunity(DWORD base)
{
	return ReadGameMemory<bool>(base + offset.m_bGunGameImmunity);
}


/* 判断当前武器是否是小刀 */
bool IsIDKnife(int m_iWeaponId)
{
	switch (m_iWeaponId)
	{
	case WEAPON_KNIFE:
		return true;
	case WEAPON_KNIFEGG:
		return true;
	case WEAPON_KNIFE_T:
		return true;
	case WEAPON_KNIFE_GUT:
		return true;
	case WEAPON_KNIFE_FLIP:
		return true;
	case WEAPON_BAYONET:
		return true;
	case WEAPON_KNIFE_M9_BAYONET:
		return true;
	case WEAPON_KNIFE_KARAMBIT:
		return true;
	case WEAPON_KNIFE_TACTICAL:
		return true;
	case WEAPON_KNIFE_BUTTERFLY:
		return true;
	case WEAPON_KNIFE_FALCHION:
		return true;
	case WEAPON_KNIFE_PUSH:
		return true;
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return true;
	default:
		return false;
	}
}
/* 判断当前武器是否是其它 */
bool IsIDOther(int m_iWeaponId)
{
	switch (m_iWeaponId)
	{
	case WEAPON_NONE:
		return true;
	case WEAPON_TASER:
		return true;
	case WEAPON_FLASHBANG:
		return true;
	case WEAPON_HEGRENADE:
		return true;
	case WEAPON_SMOKEGRENADE:
		return true;
	case WEAPON_MOLOTOV:
		return true;
	case WEAPON_DECOY:
		return true;
	case WEAPON_INCGRENADE:
		return true;
	case WEAPON_C4:
		return true;
	default:
		return false;
	}
}
/* 设置角度 */
void set_angle(D3DXVECTOR3& angle)
{
	DWORD addr = ReadGameMemory<DWORD>(engine + offset.dwClientState);
	if (addr)
	{
		WriteGameMemory<D3DXVECTOR3>(addr +offset.dwClientState_ViewAngles, angle);
	}
}


/* 获取后坐力 */
vec3 get_recoil_angle()
{
	return ReadGameMemory<vec3>(getLocalPlayer() + offset.aimPunchAngle);
}



/* 获取存活状态 */
bool get_life_state(DWORD base)
{
	int health = ReadGameMemory<int>(base + offset.health);
	unsigned char state = ReadGameMemory<unsigned char>(base + offset.m_lifeState);
	return health && !state;
}


/* 判断当前武器是否能开镜 */
bool IsIDSniper(int m_iWeaponId)
{
	switch (m_iWeaponId)
	{
	case WEAPON_AWP:
		return true;
	case WEAPON_G3SG1:
		return true;
	case WEAPON_SCAR20:
		return true;
	case WEAPON_SSG08:
		return true;
	default:
		return false;
	}
}


/* 获取开镜状态 */
bool get_scoped(DWORD base)
{
	return ReadGameMemory<bool>(base + offset.m_bIsScoped);
}