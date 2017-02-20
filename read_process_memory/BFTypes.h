#pragma once

#include <windows.h>
#include <stdexcept>

typedef float matrix4x4_t[4][4];

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

class BadName : public std::out_of_range
{
public:
	BadName() : std::out_of_range("Bad name pointer") {}
};

class BadClientSoldierEntity : public std::out_of_range
{
public:
	BadClientSoldierEntity() : std::out_of_range("Bad ClientSoldierEntity pointer") {}
};

class D3DXVECTOR3 {
public:
	D3DXVECTOR3() : x(0), y(0), z(0) {}
	D3DXVECTOR3(float x, float y, float z) : x(x), y(y), z(z) {}
	float x;
	float y;
	float z;
};

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

	class WeaponComponentData
	{
	public:
		char pad_0x0000[0x180]; //0x0000
		char* WeaponName; //0x0180 

	};

	class WeaponFiring
	{
	public:
		char pad_0x0000[0x140]; //0x0000
		WeaponComponentData* m_WeaponComponentData; //0x0140 

	};

	class ClientSoldierWeapon
	{
	public:
		char pad_0x0000[0x4A18]; //0x0000
		ClientWeapon* m_pWeapon; //0x4A18 
		char pad_0x4A20[0x10]; //0x4A20
		WeaponFiring* Primary; //0x4A30 
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

	class ClientCapturePointEntity
	{
	public:
		char pad_0x0000[0xA8]; //0x0000
		uintptr_t next; //0x00A8 
		char pad_0x00B0[0x30]; //0x00B0
		float OwnerCapturePercent; //0x00E0 
		__int32 OwnerTeam; //0x00E4 
		__int32 Defenders; //0x00E8 
		__int32 AttackingTeam; //0x00EC 
		__int16 N3DE6A3CC; //0x00F0 
		__int16 N000015A9; //0x00F2 
		char pad_0x00F4[0x22C]; //0x00F4
		matrix4x4_t Transform; //0x0320 
		char pad_0x0360[0x1B8]; //0x0360

	}; //Size=0x0518

	class ClassInfo
	{
	public:
		char pad_0x0000[0x60]; //0x0000
		uintptr_t first; //0x0060
	};

};

namespace lz
{

	class BFClientSoldierHealthComponent {
	public:
		BFClientSoldierHealthComponent(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

		float Health() const {
			uintptr_t off = offsetof(_o::BFClientSoldierHealthComponent, Health);
			return read<float>(phandle, base_addr, off);
		}

		float MaxHealth() const {
			uintptr_t off = offsetof(_o::BFClientSoldierHealthComponent, MaxHealth);
			return read<float>(phandle, base_addr, off);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientSoldierPrediction {
	public:
		ClientSoldierPrediction(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}
		D3DXVECTOR3 Position() const {
			uintptr_t off = offsetof(_o::ClientSoldierPrediction, Position);
			return read<D3DXVECTOR3>(phandle, base_addr, off);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientWeapon {
	public:
		ClientWeapon(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}
		float FOV() const {
			uintptr_t off = offsetof(_o::ClientWeapon, fov);
			return read<float>(phandle, base_addr, off);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientSoldierWeapon {
	public:
		ClientSoldierWeapon(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) { }

		ClientWeapon Weapon() const {
			uintptr_t off = offsetof(_o::ClientSoldierWeapon, m_pWeapon);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientWeapon(phandle, base);
		}

		std::string WeaponName() const {
			uintptr_t weapon_firing_off = offsetof(_o::ClientSoldierWeapon, Primary);
			uintptr_t weapon_firing_base = read<uintptr_t>(phandle, base_addr, weapon_firing_off);

			uintptr_t WeaponComponentData_off = offsetof(_o::WeaponFiring, m_WeaponComponentData);
			uintptr_t WeaponComponentData = read<uintptr_t>(phandle, weapon_firing_base, WeaponComponentData_off);

			char name[128];
			uintptr_t name_off = offsetof(_o::WeaponComponentData, WeaponName);
			uintptr_t name_ptr;
			ReadProcessMemory(phandle, (void*)(WeaponComponentData + name_off), &name_ptr, sizeof(uintptr_t), 0);
			if (!IsValidPtr(name_ptr)) {
				return "";
			}
			ReadProcessMemory(phandle, (void*)name_ptr, &name, sizeof(char) * 128, 0);
			name[127] = 0;
			return name;
		}
		
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientSoldierEntity {
	public:
		ClientSoldierEntity() : phandle(0), base_addr(0) {}
		ClientSoldierEntity(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

		float Yaw() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, yaw);
			return read<float>(phandle, base_addr, off);
		}

		float Fov() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, weapon);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientSoldierWeapon(phandle, base).Weapon().FOV();
		}

		std::string WeaponName() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, weapon);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientSoldierWeapon(phandle, base).WeaponName();
		}

		D3DXVECTOR3 Position() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, soldierPrediction);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientSoldierPrediction(phandle, base).Position();
		}

		float Health() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, healthComponent);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return BFClientSoldierHealthComponent(phandle, base).Health();
		}

		float MaxHealth() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, healthComponent);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return BFClientSoldierHealthComponent(phandle, base).MaxHealth();
		}

	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};


	class ClientPlayer {
	public:
		ClientPlayer(HANDLE phandle, uintptr_t base_addr)
				: phandle(phandle), base_addr(base_addr) {
			uintptr_t off = offsetof(_o::ClientPlayer, ClientSoldierEntity);
			uintptr_t indirect;
			ReadProcessMemory(phandle, (void*)(base_addr + off), &indirect, sizeof(uintptr_t), 0);
			if (!IsValidPtr(indirect)) {
				throw BadClientSoldierEntity{};
			}
			ReadProcessMemory(phandle, (void*)indirect, &indirect, sizeof(uintptr_t), 0);
			uintptr_t ptr_to_entity = indirect - sizeof(uintptr_t);
			clientSoldierEntity = ClientSoldierEntity(phandle, ptr_to_entity);
		}

		std::string Name() const {
			char name[128];
			uintptr_t off = offsetof(_o::ClientPlayer, Name);
			uintptr_t name_ptr;
			ReadProcessMemory(phandle, (void*)(base_addr + off), &name_ptr, sizeof(uintptr_t), 0);
			if (!IsValidPtr(name_ptr)) {
				throw BadName{};
			}
			ReadProcessMemory(phandle, (void*)name_ptr, &name, sizeof(char) * 128, 0);
			name[127] = 0;
			return name;
		}

		int Team() const {
			uintptr_t off = offsetof(_o::ClientPlayer, Team);
			return read<__int32>(phandle, base_addr, off);
		}
		bool IsVehicle() const {
			uintptr_t off = offsetof(_o::ClientPlayer, Vehicle);
			return read<uintptr_t>(phandle, base_addr, off) != 0;
		}



		float Yaw() {
			try {
				return clientSoldierEntity.Yaw();
			}
			catch (const std::out_of_range& e)
			{
				return 0.0f;
			}
		}

		float Fov() {
			try { return clientSoldierEntity.Fov(); }
			catch (const std::out_of_range& e) { return 0.0f; }
		}

		std::string WeaponName() {
			return clientSoldierEntity.WeaponName();
		}

		float Health() {
			try {
				return clientSoldierEntity.Health();
			}
			catch (const std::out_of_range& e) { return 0.0f; }
		}

		float MaxHealth() {
			try { return clientSoldierEntity.MaxHealth(); }
			catch (const std::out_of_range& e) { return 0.0f; }
		}

		D3DXVECTOR3 Position() {
			try { return clientSoldierEntity.Position(); }
			catch (const std::out_of_range& e) { return D3DXVECTOR3{}; }
		}

	private:
		HANDLE phandle;
		uintptr_t base_addr;
		ClientSoldierEntity clientSoldierEntity;
	};

	class PlayerList {
	public:
		PlayerList(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

		ClientPlayer ClientPlayerAt(int index) const {
			uintptr_t off = index * sizeof(uintptr_t);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientPlayer(phandle, base);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientPlayerManager {
	public:
		ClientPlayerManager(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

		ClientPlayer LocalPlayer() const {
			uintptr_t off = offsetof(_o::ClientPlayerManager, m_pLocalPlayer);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientPlayer(phandle, base);
		}

		PlayerList Players() const {
			uintptr_t off = offsetof(_o::ClientPlayerManager, m_playerList);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return PlayerList(phandle, base);
	} 
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientGameManager {
	public:
		ClientGameManager(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}
		ClientPlayerManager PlayerManager() const {
			uintptr_t off = offsetof(_o::ClientGameManager, m_pPlayerManager);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientPlayerManager(phandle, base);
		}
		private:
			HANDLE phandle;
			uintptr_t base_addr;
	};

	class GameClientContext {
	public:
		GameClientContext(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

		ClientGameManager GameManager() const {
			uintptr_t off = offsetof(_o::GameClientContext, m_pManager);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientGameManager(phandle, base);
		}
	protected:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientCapturePointEntity
	{
	public:
		ClientCapturePointEntity(HANDLE phandle, uintptr_t base_addr)
			: phandle(phandle), base_addr(base_addr) {}

		bool HasNext() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, next);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return base != 0;
		}

		ClientCapturePointEntity Next() const {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, next);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			base -= offsetof(_o::ClientCapturePointEntity, next);
			return ClientCapturePointEntity(phandle, base);
		}

		float OwnerCapturePercent() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, OwnerCapturePercent);
			return read<float>(phandle, base_addr, off);
		}

		__int32 OwnerTeam() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, OwnerTeam );
			return read<__int32>(phandle, base_addr, off);
		}

		__int32 Defenders() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, Defenders );
			return read<__int32>(phandle, base_addr, off);
		}

		__int32 AttackingTeam() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, AttackingTeam );
			return read<__int32>(phandle, base_addr, off);
		}
		 
		D3DXVECTOR3 Position() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, Transform );
			float obj[16];
			if (!IsValidPtr(base_addr + off)) {
				throw std::out_of_range(std::to_string(base_addr) + " + " + std::to_string(off) + " is not a valid pointer.");
			}
			ReadProcessMemory(phandle, (void*)(base_addr + off), &obj[0], sizeof(float) * 16, 0);

			return D3DXVECTOR3{ obj[12], obj[13], obj[14] };
		}

	protected:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientCapturePointEntity_ClassInfo
	{
	public:
		ClientCapturePointEntity_ClassInfo(HANDLE phandle, uintptr_t base_addr)
			: phandle(phandle), base_addr(base_addr) {}

		bool HasList() const {
			uintptr_t off = offsetof(_o::ClassInfo, first);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return base != 0;
		}

		ClientCapturePointEntity GetFirst() const {
			uintptr_t off = offsetof(_o::ClassInfo, first);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			base -= offsetof(_o::ClientCapturePointEntity, next);
			return ClientCapturePointEntity(phandle, base);
		}

	protected:
		HANDLE phandle;
		uintptr_t base_addr;
		
	};
}

