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

string slurp(ifstream& in) {
	stringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

lz::ClientPlayerManager construct_player_manager(HANDLE phandle)
{
	uintptr_t address = 0x14341b650;
	return lz::GameClientContext(phandle, address).GameManager().PlayerManager();
}

Player get_local_player(const lz::ClientPlayerManager& player_manager)
{
	Player p;
	try {
		lz::ClientPlayer local_player = player_manager.LocalPlayer();
		p.name = local_player.Name();
		p.team = local_player.Team();
		p.is_vehicle = local_player.IsVehicle();
		p.health = local_player.Health();
		p.max_health = local_player.MaxHealth();
		p.fov = local_player.Fov();
		p.yaw = local_player.Yaw();
		auto pos = local_player.Position();
		p.x = pos.x;
		p.y = pos.y;
		p.z = pos.z;
	}
	catch (const BadName& n) { }
	catch (const BadClientSoldierEntity& n) { }
	return p;
}

vector<Player> get_remote_players(const lz::ClientPlayerManager& player_manager)
{
	lz::PlayerList player_list = player_manager.Players();
	vector<Player> players;
	const int max_players = 64;
	for (int i = 0; i < max_players; i++) {
		try {
			Player p;
			lz::ClientPlayer client_player = player_list.ClientPlayerAt(i);
			p.name = client_player.Name();
			p.team = client_player.Team();
			p.is_vehicle = client_player.IsVehicle();
			p.health = client_player.Health();
			p.max_health = client_player.MaxHealth();
			p.yaw = client_player.Yaw();
			auto pos = client_player.Position();
			p.x = pos.x;
			p.y = pos.y;
			p.z = pos.z;
			players.emplace_back(p);
		}
		catch (const std::exception& e) {
		}
	}
	return players;
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
		const int tcount = team_count[t];

		result[tstr][tcount]["name"] = remote_player.name;
		result[tstr][tcount]["health"] = remote_player.health;
		result[tstr][tcount]["max_health"] = remote_player.max_health;
		result[tstr][tcount]["is_vehicle"] = remote_player.is_vehicle;
		result[tstr][tcount]["yaw"] = remote_player.yaw;
		result[tstr][tcount]["x"] = remote_player.x;
		result[tstr][tcount]["y"] = remote_player.y;
		result[tstr][tcount]["z"] = remote_player.z;
		team_count[t]++;
	}
}



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
	HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid);
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
		([phandle]{
		crow::json::wvalue result;
		try {
			result["status"] = "success";
			lz::ClientPlayerManager player_manager = construct_player_manager(phandle);
			Player player = get_local_player(player_manager);
			vector<Player> remote_players = get_remote_players(player_manager);
			add_local_player_to_result(result, player);
			add_remote_players_to_result(result, player.name, remote_players);
			return result;
		}
	    catch( const std::out_of_range & e) {
			result["status"] = "failure";
			result["reason"] = "bad player manager";
			return result;
		}

	});
	app.port(18080).multithreaded().run();
}

