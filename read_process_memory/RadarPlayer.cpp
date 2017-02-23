#include "RadarPlayer.h"
#include <map>

using namespace std;

const map<string, string> class_lookup = {
	{"AntiChars", "Assault"},
	{"Dynamite", "Assault"},
	{"TankGewehr", "Assault"},

	{"Syringe", "Medic"},
	{"Medic Bag", "Medic"},

	{"SmallAmmopack", "Support"},
	{"AmmoCrate", "Support"},
	{"Wrench", "Support"},
	{"Mortar", "Support"},
	{"RifleGrenadeLauncher", "Support"},

	{"Webbey Scott Flare Gun", "Scout"},
	{"Shield", "Scout"},
	{"Trench Periscope", "Scout"},
	{"Tripwire Bomb", "Scout"}
};

string lookup(std::string key) {
	if (class_lookup.find(key) == class_lookup.end()) {
		return "";
	} else {
		return class_lookup.at(key);
	}
}

std::string determine_class(std::string gadget1, std::string gadget2) {
	string result1 = lookup(gadget1);
	string result2 = lookup(gadget2);

	if (result1 == "" && result2 == "")
	{
		return "unknown";
	}
	return (result1 != "") ? result1 : result2;
}

namespace radar {
	Player::Player(const lz::ClientPlayer& p) {
		try { // need at least a name to count this as a valid ClientPlayer object
			read_ClientPlayer(p);
		}
		catch (const BadName& e) {
			(void)e;
			throw BadClientPlayer();
		}

		try {
			read_ClientSoldierEntity(p.clientSoldierEntity());
		}
		catch (const BadClientSoldierEntity& e) {
			(void)e;
			// If we cant read the ClientSoldierEntity then we'll return what we have.
			// The player is likely dead.
		}
	}

	void Player::read_ClientPlayer(const lz::ClientPlayer& p) {
		_name = p.name(); // might throw, indicating invalid ClientPlayer
		_team = p.team();
		_isVehicle = p.clientVehicleEntity() != 0;
	}

	void Player::read_ClientSoldierEntity(const lz::ClientSoldierEntity& cse) {
		_yaw = cse.yaw();
		_isOccluded = cse.occluded();
		try {
			read_PlayerKit(cse.clientSoldierWeaponsComponent().playerKit());
		}
		catch (const std::out_of_range& e) {
			// Maybe the PlayerKit doesnt exist when in a vehicle?
			(void)e;
		}

		try {
			read_BFClientSoldierHealthComponent(cse.bfClientSoldierHealthComponent());
		}
		catch (const std::out_of_range& e) {
			(void)e;
		}

		try {
			read_ClientSoldierWeapon(cse.activeSoldierWeapon());
		}
		catch (const std::out_of_range& e) {
			(void)e;
		}

		try {
			read_ClientSoldierPrediction(cse.clientSoldierPrediction());
		}
		catch (const std::out_of_range& e) {
			(void)e;
		}

	}

	void Player::read_ClientSoldierPrediction(const lz::ClientSoldierPrediction& p) {
		_position = p.position();
	}

	void Player::read_ClientSoldierWeapon(const lz::ClientSoldierWeapon& w) {
		_fov = w.clientWeapon().fov();

	}
	void Player::read_BFClientSoldierHealthComponent(const lz::BFClientSoldierHealthComponent& h) {
		_health = h.health();
		_maxHealth = h.maxHealth();
	}

	void Player::read_PlayerKit(const lz::PlayerKit& k) {
		// The player might not actually have anything in the playerkit slot, so the slot would
		// be a null pointer. We want to move on to other slots in that case.
		try {
			_primaryName = k.primary().weaponEntityData().weaponName();
		}
		catch (const std::out_of_range& e) {
			(void)e;
			_primaryName = "";
		}

		try {
			_secondaryName = k.secondary().weaponEntityData().weaponName();
		}
		catch (const std::out_of_range& e) {
			(void)e;
			_secondaryName = "";
		}

		try {
			_gadget1Name = k.gadget1().weaponEntityData().weaponName();
		}
		catch (const std::out_of_range& e) {
			(void)e;
			_gadget1Name = "";
		}

		try {
			_gadget2Name = k.gadget2().weaponEntityData().weaponName();
		}
		catch (const std::out_of_range& e) {
			(void)e;
			_gadget2Name = "";
		}

		// hack for detecting special classes
		// TODO: refactor this
		if (_primaryName == "MaximMG0815") {
			_playerClass = "Sentry";
		}
		else {
			_playerClass = determine_class(_gadget1Name, _gadget2Name);
		}
	}
}
