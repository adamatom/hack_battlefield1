#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "BFTypes.h"
#include "PlayerTypes.h"

using namespace std;
using namespace _o;

bool contruct_player_manager(ClientPlayerManager& pm);
void add_local_player_to_result(crow::json::wvalue& result, const Player& player);
void add_remote_players_to_result(
	crow::json::wvalue& result, const string& local_name, const vector<Player>& players);
string get_player_name(HANDLE phandle, const ClientPlayer& player);
float get_player_fov(HANDLE phandle, const ClientSoldierEntity& player_entity);
void get_player_health(HANDLE phandle, const ClientSoldierEntity& player_entity, float* o_health, float* o_max_health);
void get_player_pos(HANDLE phandle, const ClientSoldierEntity& player_entity, float* x, float* y, float* z);
bool bad_player_manager(const ClientPlayerManager& playerManager);
bool bad_player_entity(const ClientSoldierEntity& entity);
bool bad_player_name(const ClientPlayer local);
bool soldier_to_ClientSoldierEntity(HANDLE phandle, uintptr_t soldier, ClientSoldierEntity* soldier_entity);


string slurp(ifstream& in) {
	stringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}


Player get_local_player(HANDLE phandle, const ClientPlayerManager& player_manager)
{
	ClientPlayer local_player;
	ReadProcessMemory(phandle, (void*)player_manager.m_pLocalPlayer, &local_player, sizeof(ClientPlayer), 0);
	if (bad_player_name(local_player)) { return Player{}; }
	auto name = get_player_name(phandle, local_player);

	ClientSoldierEntity player_entity;
	const bool player_entity_decoded = soldier_to_ClientSoldierEntity(phandle, local_player.ClientSoldierEntity, &player_entity);
	if( !player_entity_decoded || bad_player_entity(player_entity)) { 
		return Player{ name, local_player.Team };
	}

	auto fov = get_player_fov(phandle, player_entity);
	float health, max_health;
	get_player_health(phandle, player_entity, &health, &max_health);

	float x, y, z;
	get_player_pos(phandle, player_entity, &x, &y, &z);

	const bool is_vehicle = local_player.Vehicle != (uintptr_t)0;

	return Player{ name, local_player.Team, is_vehicle, health, max_health, fov, player_entity.yaw, x, y, z };
}

vector<Player> get_remote_players(HANDLE phandle, const ClientPlayerManager& player_manager)
{
	vector<Player> players;

	PlayerList list;
	ReadProcessMemory(phandle, (void*)player_manager.m_playerList, &list, sizeof(PlayerList), 0);

	const int max_players = 64;
	for (int i = 0; i < max_players; i++) {
		if (!IsValidPtr(list.players[i])) continue;

		ClientPlayer player;
		ReadProcessMemory(phandle, (void*)list.players[i], &player, sizeof(ClientPlayer), 0);
		if (bad_player_name(player)) { continue; }
		auto name = get_player_name(phandle, player);

		ClientSoldierEntity player_entity;
		const bool player_entity_decoded = soldier_to_ClientSoldierEntity(phandle, player.ClientSoldierEntity, &player_entity);
		if (!player_entity_decoded || bad_player_entity(player_entity)) {
			continue;
		}

		float health, max_health;
		get_player_health(phandle, player_entity, &health, &max_health);

		float x, y, z;
		get_player_pos(phandle, player_entity, &x, &y, &z);

		const bool is_vehicle = player.Vehicle != (uintptr_t)0;

		// Not a big deal if the health and position go a little screwy, no chance for segfault or anything. Let the UI decide what is good or bad.
		players.emplace_back(Player{ name, player.Team, is_vehicle, health, max_health, x, y, z });
	}

	return players;
}



HANDLE phandle;


int main()
{

	HWND hwnd = FindWindow(NULL, L"Battlefield™ 1");
	if (!hwnd)
	{
		cout << "Window not found!\n";
		cin.get();
		return 0;
	}

	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	phandle = OpenProcess(PROCESS_VM_READ, 0, pid);
	if (!phandle)
	{
		cout << "Could not get handle!\n";
		cin.get();
		return 0;
	}


	crow::SimpleApp app;

	CROW_ROUTE(app, "/index.html")
		([] {
		ifstream myfile("index.html");
		string render = slurp(myfile);
		myfile.close();
		return render;
	});

	CROW_ROUTE(app, "/drawmap.js")
		([] {
		ifstream myfile("drawmap.js");
		string render = slurp(myfile);
		myfile.close();
		return render;
	});

	CROW_ROUTE(app, "/report")
		([]{
		crow::json::wvalue result;
		ClientPlayerManager player_manager;

		if (!contruct_player_manager(player_manager)) {
			result["status"] = "failure";
			result["reason"] = "bad player manager";
			return result;
		}

		result["status"] = "success";
		Player player = get_local_player(phandle, player_manager);
		vector<Player> remote_players = get_remote_players(phandle, player_manager);
		add_local_player_to_result(result, player);
		add_remote_players_to_result(result, player.name, remote_players);
		return result;
	});
	app.port(18080).multithreaded().run();
}

template<typename T>
T read(uintptr_t base, uintptr_t off) {
	T obj;
	ReadProcessMemory(phandle, (void*)(base + off), &obj, sizeof(T), 0);
	return obj;
}

bool contruct_player_manager(ClientPlayerManager& pm)
{
	uintptr_t address = 0x14341b650;
	_o::GameClientContext ctxt;
	_o::ClientGameManager mgr;


	lz::GameClientContext lz_ctxt(phandle, address);
	lz::ClientPlayer lz_p = lz_ctxt.m_pManager().m_pPlayerManager().m_pLocalPlayer();
	auto test1 = lz_p.Team();
	auto test2 = lz_p.Name();

	ReadProcessMemory(phandle, (void*)address, &ctxt, sizeof(_o::GameClientContext), 0);
	ReadProcessMemory(phandle, (void*)ctxt.m_pManager, &mgr, sizeof(_o::ClientGameManager), 0);
	ReadProcessMemory(phandle, (void*)mgr.m_pPlayerManager, &pm, sizeof(_o::ClientPlayerManager), 0);
	return bad_player_manager(pm) == false;
}

void add_local_player_to_result(crow::json::wvalue& result, const Player& player)
{
	result["local_player"]["team"] = player.team;
	result["local_player"]["name"] = player.name;
	result["local_player"]["health"] = player.health;
	result["local_player"]["max_health"] = player.max_health;
	result["local_player"]["fov"] = player.fov;
	result["local_player"]["yaw"] = player.yaw;
	result["local_player"]["is_vehicle"] = player.is_vehicle;
}

void add_remote_players_to_result(
	crow::json::wvalue& result, const string& local_name, const vector<Player>& players)
{
	int team_count[3] = { 0,0,0 };
	for (const auto& remote_player : players) {
		const int t = remote_player.team;
		// hack: This list's copy of the player has the location, so we patch it in at this point.
		if (remote_player.name == local_name) { 
			result["local_player"]["x"] = remote_player.x;
			result["local_player"]["y"] = remote_player.y;
			result["local_player"]["z"] = remote_player.z;
		}
		if (t != 1 && t != 2) { continue; } // bad object snuck through
		const string tstr = "team" + to_string(t);
		const int tcount = team_count[t]; // likes to crash reading this array

		result[tstr][tcount]["name"] = remote_player.name;
		result[tstr][tcount]["health"] = remote_player.health;
		result[tstr][tcount]["max_health"] = remote_player.max_health;
		result[tstr][tcount]["is_vehicle"] = remote_player.is_vehicle;
		result[tstr][tcount]["x"] = remote_player.x;
		result[tstr][tcount]["y"] = remote_player.y;
		result[tstr][tcount]["z"] = remote_player.z;
		team_count[t]++;
	}
}

string get_player_name(HANDLE phandle, const ClientPlayer& player)
{
	char name[128];
	ReadProcessMemory(phandle, (void*)player.Name, &name, sizeof(char) * 128, 0);
	name[127] = 0;
	return name;
}

float get_player_fov(HANDLE phandle, const ClientSoldierEntity& player_entity)
{
	ClientSoldierWeapon client_soldier_weapon;
	if (!IsValidPtr(player_entity.weapon)) { return 0; }

	ReadProcessMemory(phandle, (void*)player_entity.weapon, &client_soldier_weapon, sizeof(ClientSoldierWeapon), 0);
	if (!IsValidPtr(client_soldier_weapon.m_pWeapon)) { return 0; }

	ClientWeapon player_weapon;
	ReadProcessMemory(phandle, (void*)client_soldier_weapon.m_pWeapon, &player_weapon, sizeof(ClientWeapon), 0);
	return player_weapon.fov;
}

void get_player_health(HANDLE phandle, const ClientSoldierEntity& player_entity, float* o_health, float* o_max_health)
{
	if (!IsValidPtr(player_entity.healthComponent))
	{
		*o_health = *o_max_health = 0.0f;
		return;
	}

	BFClientSoldierHealthComponent player_health;
	ReadProcessMemory(phandle, (void*)player_entity.healthComponent, &player_health, sizeof(BFClientSoldierHealthComponent), 0);
	*o_health = player_health.Health;
	*o_max_health = player_health.MaxHealth;
}

void get_player_pos(HANDLE phandle, const ClientSoldierEntity& player_entity, float* x, float* y, float* z)
{
	if (!IsValidPtr(player_entity.soldierPrediction))
	{
		*x = *y = *z = 0.0f;
		return;
	}

	ClientSoldierPrediction player_phys;
	ReadProcessMemory(phandle, (void*)player_entity.soldierPrediction, &player_phys, sizeof(ClientSoldierPrediction), 0);
	*x = player_phys.Position.x;
	*y = player_phys.Position.y;
	*z = player_phys.Position.z;
}

bool soldier_to_ClientSoldierEntity(HANDLE phandle, uintptr_t soldier, ClientSoldierEntity* soldier_entity)
{
	if (!IsValidPtr(soldier)) { return false; }
	uintptr_t indirect;
	ReadProcessMemory(phandle, (void*)soldier, &indirect, sizeof(uintptr_t), 0);

	uintptr_t ptr_to_entity = indirect - sizeof(uintptr_t);
	if (!IsValidPtr(ptr_to_entity)) { return false; }
	ReadProcessMemory(phandle,(void*)ptr_to_entity, soldier_entity, sizeof(ClientSoldierEntity), 0);

	return !bad_player_entity(*soldier_entity);
}

bool bad_player_manager(const ClientPlayerManager& playerManager)
{
	return !IsValidPtr(playerManager.m_pLocalPlayer) || !IsValidPtr(playerManager.m_playerList);
}

bool bad_player_name(const ClientPlayer local)
{
	return !IsValidPtr(local.Name);
}

bool bad_player_entity(const ClientSoldierEntity& entity)
{
	// Some of the code does use other pointers found in the entity, but we consider them optional, 
	// so they can be bad without the entire ClientSoldierEntity being declared bad.
	return !IsValidPtr(entity.healthComponent) || !IsValidPtr(entity.soldierPrediction);
}


