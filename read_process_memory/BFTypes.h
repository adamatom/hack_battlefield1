#pragma once

#include <windows.h>
#include <stdexcept>

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

typedef struct D3DXVECTOR3 {
	float x;
	float y;
	float z;
} D3DXVECTOR3, *LPD3DXVECTOR3;

namespace _o
{
	class GameClientContext;
	class ClientGameManager;
	class ClientPlayerManager;
	class PlayerList;
	class ClientPlayer;
	class ClientSoldierEntity;
	class ClientSoldierPrediction;
	class BFClientSoldierHealthComponent;
	class ClientSoldierWeapon;
	class ClientWeapon;

	class GameClientContext
	{
	public:
		ClientGameManager* m_pManager; //0x0000 
	};

	class ClientGameManager
	{
	public:
		char _0x0000[32];
		ClientPlayerManager* m_pPlayerManager; //0x0020 
	};

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
		char _0x0000[24];
		char* Name; //0x0018 
		char _0x0020[7184];
		__int32 pad1c30; //0x1C30 
		__int32 Team; //0x1C34 
		char _0x1C38[240];
		uintptr_t ClientSoldierEntity; // 0x1d28 this requires special deciphering to get at, it is a smart pointer or something. See soldier_to_ClientSolderEntity function
		char _0x1D30[8];
		uintptr_t Vehicle; //0x1D38 if not null then this player is a vehicle

	};

	class ClientSoldierEntity
	{
	public:
		char _0x0000[448];
		BFClientSoldierHealthComponent* healthComponent; //0x01C0 
		char _0x01C8[976];
		ClientSoldierPrediction* soldierPrediction; //0x0598
		char _0x05A0[16];
		__int32 pad4;
		float yaw; //0x05B4 
		char _0x05B8[180];
		__int32 pad066c; //0x066C 
		ClientSoldierWeapon* weapon; //0x0670 
	};

	class ClientSoldierWeapon
	{
	public:
		char _0x0000[18968];
		ClientWeapon* m_pWeapon; //0x4A18 
	};

	class ClientWeapon
	{
	public:
		char _0x0000[816];
		float fov; //0x0330 
	};

	class ClientSoldierPrediction
	{
	public:
		char _0x0000[64];
		D3DXVECTOR3 Position; //0x0040 
		char _0x004C[20];
		D3DXVECTOR3 Velocity; //0x0060 
	};

	class BFClientSoldierHealthComponent
	{
	public:
		char _0x0000[32];
		float Health; //0x0020 
		float MaxHealth; //0x0024 
	};

};

namespace lz
{
#define BEGIN_LAZY_READER(TYPE)\
	class TYPE { \
	public: \
		TYPE##(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

#define END_LAZY_READER() \
	private: \
		HANDLE phandle; \
		uintptr_t base_addr; \
	};

#define GET_POINTER_FIELD(TYPE, MEMBER, RET_TYPE)\
	lz::##RET_TYPE MEMBER() const { \
			uintptr_t off = offsetof(_o::##TYPE, MEMBER); \
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off); \
			return lz::##RET_TYPE(phandle, base);\
	}

#define GET_DATA_FIELD(TYPE, MEMBER, RET_TYPE)\
	RET_TYPE MEMBER() const { \
			uintptr_t off = offsetof(_o::##TYPE, MEMBER); \
			return read<RET_TYPE>(phandle, base_addr, off); \
	}


/* Much boilerplate on these classes that looked like the following. Macros cut out the boilerplate	
class GameClientContext
	{
	public:
		GameClientContext(HANDLE phandle, uintptr_t base_addr)
			: phandle(phandle), base_addr(base_addr) {}

		lz::ClientGameManager GetClientGameManager() const
		{
			uintptr_t off = offsetof(_o::GameClientContext, m_pManager);
			uintptr_t cgm_base = read<uintptr_t>(base_addr, off);
			return lz::ClientGameManager(phandle, cgm_base);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};*/

	BEGIN_LAZY_READER(BFClientSoldierHealthComponent)
		GET_DATA_FIELD(BFClientSoldierHealthComponent, Health, float)
		GET_DATA_FIELD(BFClientSoldierHealthComponent, MaxHealth, float)
	END_LAZY_READER()

	BEGIN_LAZY_READER(ClientSoldierPrediction)
		GET_DATA_FIELD(ClientSoldierPrediction, Position, D3DXVECTOR3)
	END_LAZY_READER()

	BEGIN_LAZY_READER(ClientWeapon)
		GET_DATA_FIELD(ClientWeapon, fov, float)
	END_LAZY_READER()
	
	BEGIN_LAZY_READER(ClientSoldierWeapon)
		GET_POINTER_FIELD(ClientSoldierWeapon, m_pWeapon, ClientWeapon)
	END_LAZY_READER()

	BEGIN_LAZY_READER(ClientSoldierEntity)
		GET_POINTER_FIELD(ClientSoldierEntity, healthComponent, BFClientSoldierHealthComponent)
		GET_POINTER_FIELD(ClientSoldierEntity, soldierPrediction, ClientSoldierPrediction)
		GET_DATA_FIELD(ClientSoldierEntity, yaw, float)
		GET_POINTER_FIELD(ClientSoldierEntity, weapon, ClientSoldierWeapon)
	END_LAZY_READER()


	BEGIN_LAZY_READER(ClientPlayer)
		std::string Name() const {
			char name[128];
			uintptr_t off = offsetof(_o::ClientPlayer, Name);
			uintptr_t name_ptr;
			ReadProcessMemory(phandle, (void*)(base_addr + off), &name_ptr, sizeof(uintptr_t), 0);
			ReadProcessMemory(phandle, (void*)name_ptr, &name, sizeof(char)*128, 0);
			name[127] = 0;
			return name;
		}
		GET_DATA_FIELD(ClientPlayer, Team, __int32)
		GET_DATA_FIELD(ClientPlayer, ClientSoldierEntity, uintptr_t)
		GET_DATA_FIELD(ClientPlayer, Vehicle, uintptr_t)
	END_LAZY_READER()

	BEGIN_LAZY_READER(PlayerList)
		lz::ClientPlayer ClientPlayer(int index) const {
			uintptr_t off = index*sizeof(uintptr_t);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return lz::ClientPlayer(phandle, base);
		}
	END_LAZY_READER()

	BEGIN_LAZY_READER(ClientPlayerManager)
		GET_POINTER_FIELD(ClientPlayerManager, m_playerList, PlayerList)
		GET_POINTER_FIELD(ClientPlayerManager, m_pLocalPlayer, ClientPlayer)
	END_LAZY_READER()

	BEGIN_LAZY_READER(ClientGameManager)
		GET_POINTER_FIELD(ClientGameManager, m_pPlayerManager, ClientPlayerManager)
	END_LAZY_READER()

	BEGIN_LAZY_READER(GameClientContext)
		GET_POINTER_FIELD(GameClientContext, m_pManager, ClientGameManager)
	END_LAZY_READER()




}

