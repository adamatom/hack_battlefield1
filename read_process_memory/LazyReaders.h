#pragma once
#include <string>
#include "BFTypes.h"
#include "Exceptions.h"


// These objects are models that read memory in a lazy way. The idea is to not use the
// BFTypes directly, but only use them for offsets, and have these lazy readers reach into
// the BF1 memory space selectively to minimize time spent in memcpy. These are lazy because
// they dont actively reach into memory until you call a method.
namespace lz
{

	class BFClientSoldierHealthComponent {
	public:
		BFClientSoldierHealthComponent(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

		float health() const {
			uintptr_t off = offsetof(_o::BFClientSoldierHealthComponent, Health);
			return read<float>(phandle, base_addr, off);
		}

		float maxHealth() const {
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
		D3DXVECTOR3 position() const {
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
		float fov() const {
			uintptr_t off = offsetof(_o::ClientWeapon, fov);
			return read<float>(phandle, base_addr, off);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class WeaponEntityData {
	public:
		WeaponEntityData(HANDLE phandle, uintptr_t base_addr)
			: phandle(phandle), base_addr(base_addr) {
		}

		int weaponClass() const {
			uintptr_t off = offsetof(_o::WeaponEntityData, weaponClass);
			return read<__int32>(phandle, base_addr, off);
		}

		std::string weaponName() const {
			char name[128];
			uintptr_t name_off = offsetof(_o::WeaponEntityData, weaponName);
			uintptr_t name_ptr;
			ReadProcessMemory(phandle, (void*)(base_addr + name_off), &name_ptr, sizeof(uintptr_t), 0);
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

	class ClientSoldierWeapon {
	public:
		ClientSoldierWeapon(HANDLE phandle, uintptr_t base_addr) 
			: phandle(phandle), base_addr(base_addr) { 
		}

		ClientWeapon clientWeapon() const {
			uintptr_t off = offsetof(_o::ClientSoldierWeapon, clientWeapon);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientWeapon(phandle, base);
		}

		WeaponEntityData weaponEntityData() const {
			uintptr_t off = offsetof(_o::ClientSoldierWeapon, weaponEntityData);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return WeaponEntityData(phandle, base);
		}
		
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class PlayerKit {
	public:
		PlayerKit(HANDLE phandle, uintptr_t base_addr) 
			: phandle(phandle), base_addr(base_addr) { 
		}

		ClientSoldierWeapon primary() const {
			return getOffset(offsetof(_o::PlayerKit, Primary));
		}

		ClientSoldierWeapon secondary() const {
			return getOffset(offsetof(_o::PlayerKit, Secondary));
		}

		ClientSoldierWeapon gadget1() const {
			return getOffset(offsetof(_o::PlayerKit, Gadget1));
		}

		ClientSoldierWeapon gadget2() const {
			return getOffset(offsetof(_o::PlayerKit, Gadget2));
		}

		ClientSoldierWeapon grenade() const {
			return getOffset(offsetof(_o::PlayerKit, Grenade));
		}

		ClientSoldierWeapon knife() const {
			return getOffset(offsetof(_o::PlayerKit, Knife));
		}

		ClientSoldierWeapon mask() const {
			return getOffset(offsetof(_o::PlayerKit, Mask));
		}

	private:
		ClientSoldierWeapon getOffset(uintptr_t offset) const {
			uintptr_t base = read<uintptr_t>(phandle, base_addr, offset);
			return ClientSoldierWeapon(phandle, base);
		}
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientSoldierWeaponsComponent
	{
	public:
		ClientSoldierWeaponsComponent(HANDLE phandle, uintptr_t base_addr)
			: phandle(phandle), base_addr(base_addr) {
		}

		PlayerKit playerKit() const {
			uintptr_t off = offsetof(_o::ClientSoldierWeaponsComponent, playerKit);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return PlayerKit(phandle, base);
		}

		int activeWeaponSlot() const {
			uintptr_t off = offsetof(_o::ClientSoldierWeaponsComponent, ActiveWeaponSlot);
			return read<__int32>(phandle, base_addr, off);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientSoldierEntity {
	public:
		ClientSoldierEntity() : phandle(0), base_addr(0) {}
		ClientSoldierEntity(HANDLE phandle, uintptr_t base_addr)
			: phandle(phandle), base_addr(base_addr) {
		}

		BFClientSoldierHealthComponent bfClientSoldierHealthComponent() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, healthComponent);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return BFClientSoldierHealthComponent(phandle, base);
		}

		ClientSoldierPrediction clientSoldierPrediction() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, clientSoldierPrediction);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientSoldierPrediction(phandle, base);
		}

		float yaw() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, yaw);
			return read<float>(phandle, base_addr, off);
		}

		ClientSoldierWeaponsComponent clientSoldierWeaponsComponent() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, soldierWeaponsComponent);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientSoldierWeaponsComponent(phandle, base);

		}

		ClientSoldierWeapon activeSoldierWeapon() const {
			uintptr_t off = offsetof(_o::ClientSoldierEntity, activeSoldierWeapon);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientSoldierWeapon(phandle, base);
		}
	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class ClientPlayer {
	public:
		ClientPlayer(HANDLE phandle, uintptr_t base_addr)
				: phandle(phandle), base_addr(base_addr) { }

		std::string name() const {
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

		int team() const {
			uintptr_t off = offsetof(_o::ClientPlayer, Team);
			return read<__int32>(phandle, base_addr, off);
		}

		ClientSoldierEntity clientSoldierEntity() const {
			uintptr_t indirect, off = offsetof(_o::ClientPlayer, clientSoldierEntity);
			ReadProcessMemory(phandle, (void*)(base_addr + off), &indirect, sizeof(uintptr_t), 0);
			if (!IsValidPtr(indirect)) {
				throw BadClientSoldierEntity{};
			}
			ReadProcessMemory(phandle, (void*)indirect, &indirect, sizeof(uintptr_t), 0);
			uintptr_t ptr_to_entity = indirect - sizeof(uintptr_t);
			return ClientSoldierEntity(phandle, ptr_to_entity);
		}

		// Choosing not to model this entity, it only contains a transform matrix that I dont wnat to use (yet).
		uintptr_t clientVehicleEntity() const {
			uintptr_t off = offsetof(_o::ClientPlayer, clientVehicleEntity);
			return read<uintptr_t>(phandle, base_addr, off);
		}

		//Probably better to always use clientSoldierEntity, this pointer has modes of validty 
		ClientSoldierEntity clientControllableEntity() const {
			uintptr_t off = offsetof(_o::ClientPlayer, clientControllableEntity);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientSoldierEntity(phandle, base);
		}

	private:
		HANDLE phandle;
		uintptr_t base_addr;
	};

	class PlayerList {
	public:
		PlayerList(HANDLE phandle, uintptr_t base_addr) : phandle(phandle), base_addr(base_addr) {}

		ClientPlayer clientPlayerAt(int index) const {
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

		ClientPlayer localPlayer() const {
			uintptr_t off = offsetof(_o::ClientPlayerManager, m_pLocalPlayer);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return ClientPlayer(phandle, base);
		}

		PlayerList players() const {
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
		ClientPlayerManager playerManager() const {
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

		ClientGameManager gameManager() const {
			uintptr_t off = offsetof(_o::GameClientContext, m_pGameManager);
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

		bool hasNext() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, next);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			return base != 0;
		}

		ClientCapturePointEntity next() const {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, next);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			base -= offsetof(_o::ClientCapturePointEntity, next);
			return ClientCapturePointEntity(phandle, base);
		}

		float ownerCapturePercent() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, OwnerCapturePercent);
			return read<float>(phandle, base_addr, off);
		}

		__int32 ownerTeam() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, OwnerTeam );
			return read<__int32>(phandle, base_addr, off);
		}

		__int32 defenders() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, Defenders );
			return read<__int32>(phandle, base_addr, off);
		}

		__int32 attackingTeam() {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity, AttackingTeam );
			return read<__int32>(phandle, base_addr, off);
		}
		 
		D3DXVECTOR3 position() {
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

		ClientCapturePointEntity first() const {
			uintptr_t off = offsetof(_o::ClientCapturePointEntity_ClassInfo, first);
			uintptr_t base = read<uintptr_t>(phandle, base_addr, off);
			if (!base) {
				throw NoCapturePointEntityExists();
			}
			base -= offsetof(_o::ClientCapturePointEntity, next);
			return ClientCapturePointEntity(phandle, base);
		}

	protected:
		HANDLE phandle;
		uintptr_t base_addr;
		
	};
}
