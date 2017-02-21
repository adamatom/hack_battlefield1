#pragma once

#include <windows.h>


template <typename T>
bool IsValidPtr(T p)
{
	const void* ptr_max_value = (void*)0x000F000000000000;
	const bool not_too_small = reinterpret_cast<void*>(p) >= reinterpret_cast<void*>(0x10000);
	const bool not_too_big = reinterpret_cast<void*>(p) < ptr_max_value;
	return  not_too_small && not_too_big;
}

template<typename T>
T read(HANDLE phandle, uintptr_t base, uintptr_t off) {
	T obj;
	if (!IsValidPtr(base + off)) {
		throw std::out_of_range(std::to_string(base) + " + " + std::to_string(off) + " is not a valid pointer.");
	}
	ReadProcessMemory(phandle, (void*)(base + off), &obj, sizeof(T), 0);
	return obj;
}


class D3DXVECTOR3 {
public:
	D3DXVECTOR3() : x(0), y(0), z(0) {}
	D3DXVECTOR3(float x, float y, float z) : x(x), y(y), z(z) {}
	float x;
	float y;
	float z;
};

typedef float matrix4x4_t[4][4];

// These are the classes reversed from the Battlefield 1 application. These exist to maintain
// the class offsets. You could read one of these structures out just fine, but some of these
// are very big (20kb in one case), which is a waste to read when you only want to read a single
// pointer value. Use the lazy readers class in-place of these.
namespace _o
{
class GameClientContext;
class ClientGameManager;
class ClientPlayerManager;
class PlayerList;
class ClientPlayer;
class ClientSoldierEntity;
class ClientSoldierPrediction;
class ClientSoldierWeapon;
class ClientWeapon;
class ClientCapturePointEntity;
class weakptr_ClientSoldierEntity;
class ClientVehicleEntity;
class TransformContainer;
class BFClientSoldierHealthComponent;
class ClientCapturePointEntity_ClassInfo;
class ClientCapturePointEntityData;
class ClientSoldierWeaponsComponent;
class PlayerKit;
class WeaponEntityData;

class GameClientContext
{
public:
	ClientGameManager* m_pGameManager; //0x0000 

}; //Size=0x0008

class ClientGameManager
{
public:
	char pad_0x0000[0x20]; //0x0000
	ClientPlayerManager* m_pPlayerManager; //0x0020 

}; //Size=0x0028

class ClientPlayerManager
{
public:
    char _0x0000[256];
    PlayerList* m_playerList; //0x0100 
    char _0x0108[1096];
    PlayerList* m_ppPlayers_0; //0x0550 
    char _0x0558[24];
    PlayerList* m_ppPlayers_1; //0x0570 
    ClientPlayer* m_pLocalPlayer; //0x0578 
};

class PlayerList
{
public:
	ClientPlayer* players[64]; //0x0000
};

class ClientPlayer
{
public:
	char pad_0x0000[0x18]; //0x0000
	char* Name; //0x0018 
	char pad_0x0020[0x1C08]; //0x0020
	__int32 pad1c30; //0x1C28 
	__int32 Team; //0x1C2C 
	char pad_0x1C30[0xF8]; //0x1C30
	weakptr_ClientSoldierEntity* clientSoldierEntity; //0x1D28 
	char pad_0x1D30[0x8]; //0x1D30
	ClientVehicleEntity* clientVehicleEntity; //0x1D38 
	char pad_0x1D40[0x8]; //0x1D40
	ClientSoldierEntity* clientControllableEntity; //0x1D48 

}; //Size=0x1D50

class ClientSoldierEntity
{
public:
	char pad_0x0000[0x1C0]; //0x0000
	BFClientSoldierHealthComponent* healthComponent; //0x01C0 
	char pad_0x01C8[0x3D0]; //0x01C8
	ClientSoldierPrediction* clientSoldierPrediction; //0x0598 
	char pad_0x05A0[0x10]; //0x05A0
	__int32 pad05b0; //0x05B0 
	float yaw; //0x05B4 
	char pad_0x05B8[0x90]; //0x05B8
	ClientSoldierWeaponsComponent* soldierWeaponsComponent; //0x0648 
	char pad_0x0650[0x20]; //0x0650
	ClientSoldierWeapon* activeSoldierWeapon; //0x0670 

}; //Size=0x0678

class ClientSoldierPrediction
{
public:
    char _0x0000[64];
    D3DXVECTOR3 Position; //0x0040 
    char _0x004C[20];
    D3DXVECTOR3 Velocity; //0x0060 
};

class ClientSoldierWeapon
{
public:
	char pad_0x0000[0x30]; //0x0000
	WeaponEntityData* weaponEntityData; //0x0030 
	char pad_0x0038[0x49E0]; //0x0038
	ClientWeapon* clientWeapon; //0x4A18 
	char pad_0x4A20[0x18]; //0x4A20

}; //Size=0x4A38

class ClientWeapon
{
public:
	char pad_0x0000[0x330]; //0x0000
	float fov; //0x0330 

};

class ClientCapturePointEntity
{
public:
	char pad_0x0000[0x30]; //0x0000
	ClientCapturePointEntityData* clientCapturePointEntityData; //0x0030 
	char pad_0x0038[0x70]; //0x0038
	ClientCapturePointEntity* next; //0x00A8 
	char pad_0x00B0[0x30]; //0x00B0
	float OwnerCapturePercent; //0x00E0 
	__int32 OwnerTeam; //0x00E4 
	__int32 Defenders; //0x00E8 
	__int32 AttackingTeam; //0x00EC 
	__int8 N000015AC; //0x00F0 
	__int8 N000014D3; //0x00F1 
	__int8 N000015AA; //0x00F2 
	__int8 N000014D6; //0x00F3 
	char pad_0x00F4[0x60]; //0x00F4
	char* PointName; //0x0154 
	char pad_0x015C[0x1C0]; //0x015C
	matrix4x4_t Transform; //0x031C 

}; //Size=0x035C

class weakptr_ClientSoldierEntity
{
public:
	char pad_0x0000[0x8]; //0x0000

}; //Size=0x0008

class ClientVehicleEntity
{
public:
	char pad_0x0000[0x38]; //0x0000
	TransformContainer* m_ptr_to_transform; //0x0038 
	char pad_0x0040[0xCC8]; //0x0040

}; //Size=0x0D08

class TransformContainer
{
public:
	char pad_0x0000[0x750]; //0x0000
	matrix4x4_t Transform; //0x0750 
	char pad_0x0790[0x78]; //0x0790

}; //Size=0x0808

class ClientCapturePointEntity_ClassInfo
{
public:
	char pad_0x0000[0x60]; //0x0000
	ClientCapturePointEntity* first; //0x0060 
	char pad_0x0068[0x18]; //0x0068

}; //Size=0x0080

class ClientCapturePointEntityData
{
public:
	char pad_0x0000[0xA8]; //0x0000
	__int32 MaybeTimeToGetControl; //0x00A8 
	__int32 MaybeTimeToLoseControl; //0x00AC 
	char pad_0x00B0[0x68]; //0x00B0
	char* SID; //0x0118 
	char* N000014F9; //0x0120 
	char pad_0x0128[0xB0]; //0x0128
	char* N00001514; //0x01D8 
	char* N00001515; //0x01E0 
	char pad_0x01E8[0x5F0]; //0x01E8

}; //Size=0x07D8

class ClientSoldierWeaponsComponent
{
public:
	char pad_0x0000[0x8A8]; //0x0000
	PlayerKit* playerKit; //0x08A8 
	char pad_0x08B0[0xB0]; //0x08B0
	__int32 ActiveWeaponSlot; //0x0960 

}; //Size=0x0964

class PlayerKit
{
public:
	ClientSoldierWeapon* Primary; //0x0000 
	ClientSoldierWeapon* Secondary; //0x0008 
	ClientSoldierWeapon* Gadget1; //0x0010 
	ClientSoldierWeapon* Mask; //0x0018 
	char pad_0x0020[0x8]; //0x0020
	ClientSoldierWeapon* Gadget2; //0x0028 
	ClientSoldierWeapon* Grenade; //0x0030 
	ClientSoldierWeapon* Knife; //0x0038 

}; //Size=0x0040

class BFClientSoldierHealthComponent
{
public:
	char _0x0000[32];
	float Health; //0x0020 
	float MaxHealth; //0x0024 
};
class WeaponEntityData
{
public:
	char pad_0x0000[0x80]; //0x0000
	__int32 weaponClass; //0x0080 
	char pad_0x0084[0xFC]; //0x0084
	char* weaponName; //0x0180 

}; //Size=0x0188


};