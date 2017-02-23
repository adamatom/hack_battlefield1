#pragma once
#include "Exceptions.h"
#include "LazyReaders.h"

namespace radar {
	// This provides a more convenient interface to all the parts that make up a player.
	// It tries very hard to return whatever is known about a player
	class Player {
	public:
		Player(const lz::ClientPlayer& p);

		std::string name() const { return _name; }
		int team() const { return _team; }
		std::string playerClass() const { return _playerClass; }
		std::string primaryName() const { return _primaryName; }
		std::string secondaryName() const { return _secondaryName; }
		std::string gadget1Name() const { return _gadget1Name; }
		std::string gadget2Name() const { return _gadget2Name; }
		float health() const { return _health; }
		float maxHealth() const { return _maxHealth; }
		bool isVehicle() const { return _isVehicle; }
		float yaw() const { return _yaw; }
		float fov() const { return _fov; }
		bool isOccluded() const { return _isOccluded; }
		D3DXVECTOR3 position() const { return _position; }

	private:
		void read_ClientPlayer(const lz::ClientPlayer& p);
		void read_ClientSoldierEntity(const lz::ClientSoldierEntity& cse);
		void read_PlayerKit(const lz::PlayerKit& k);
		void read_BFClientSoldierHealthComponent(const lz::BFClientSoldierHealthComponent& h);
		void read_ClientSoldierWeapon(const lz::ClientSoldierWeapon& w);
		void read_ClientSoldierPrediction(const lz::ClientSoldierPrediction& p);
		std::string _name;
		int _team = 0;
		std::string _playerClass = "unknown";
		std::string _primaryName = "unknown";
		std::string _secondaryName = "unknown";
		std::string _gadget1Name = "unknown";
		std::string _gadget2Name = "unknown";
		float _health = 0.0f;
		float _maxHealth = 0.0f;
		bool _isVehicle = false;
		float _yaw = 0.0f;
		float _fov = 0.0f;
		bool _isOccluded = false;
		D3DXVECTOR3 _position;
	};
}
