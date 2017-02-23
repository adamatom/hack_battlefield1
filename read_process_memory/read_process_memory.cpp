#include "stdafx.h"

#pragma warning(push, 0)
#include "crow.h"
#pragma warning(pop)

#include <windows.h>
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "BFTypes.h"
#include "LazyReaders.h"
#include "PlayerTypes.h"
#include "RadarPlayer.h"


using namespace std;

string slurp(ifstream& in) {
	stringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

lz::ClientPlayerManager construct_player_manager(HANDLE phandle) {
	uintptr_t address = 0x143436980;
	return lz::GameClientContext(phandle, address).gameManager().playerManager();
}

std::string read_local_player(const lz::ClientPlayerManager& player_manager, crow::json::wvalue& result) {
	try {
		radar::Player player(player_manager.localPlayer());
		auto name = player.name();
		result["local_player"]["name"] = name;
		result["local_player"]["team"] = player.team();
		result["local_player"]["playerClass"] = player.playerClass();
		result["local_player"]["primaryName"] = player.primaryName();
		result["local_player"]["secondaryName"] = player.secondaryName();
		result["local_player"]["gadget1Name"] = player.gadget1Name();
		result["local_player"]["gadget2Name"] = player.gadget2Name();
		result["local_player"]["health"] = player.health();
		result["local_player"]["maxHealth"] = player.maxHealth();
		result["local_player"]["isVehicle"] = player.isVehicle();
		result["local_player"]["yaw"] = player.yaw();
		result["local_player"]["fov"] = player.fov();
		return name;
	}
	catch (const BadClientPlayer& e) {
		(void)e;
		return "";
	}
}

void read_remote_players(const lz::ClientPlayerManager& player_manager, crow::json::wvalue& result, string local_name) {
	lz::PlayerList player_list = player_manager.players();
	const int max_players = 64;
	int team_count[3] = { 0,0,0 };
	for (int i = 0; i < max_players; i++) {
		try {
			radar::Player player(player_list.clientPlayerAt(i));
			auto name = player.name();
			const int t = player.team();
			// hack: This list's copy of the player has the location, so we patch it in at this point.
			if (name == local_name) {
				auto pos = player.position();
				result["local_player"]["x"] = pos.x;
				result["local_player"]["y"] = pos.y;
				result["local_player"]["z"] = pos.z;
				continue; // also dont send the local player in the team list
			}
			if (t != 1 && t != 2) { continue; }
			const string tstr = "team" + to_string(t);
			const int tcount = team_count[t];

			result[tstr][tcount]["name"] = name;
			result[tstr][tcount]["playerClass"] = player.playerClass();
			result[tstr][tcount]["primaryName"] = player.primaryName();
			result[tstr][tcount]["secondaryName"] = player.secondaryName();
			result[tstr][tcount]["gadget1Name"] = player.gadget1Name();
			result[tstr][tcount]["gadget2Name"] = player.gadget2Name();
			result[tstr][tcount]["health"] = player.health();
			result[tstr][tcount]["maxHealth"] = player.maxHealth();
			result[tstr][tcount]["isVehicle"] = player.isVehicle();
			result[tstr][tcount]["yaw"] = player.yaw();
			result[tstr][tcount]["occluded"] = player.isOccluded();
			auto pos = player.position();
			result[tstr][tcount]["x"] = pos.x;
			result[tstr][tcount]["y"] = pos.y;
			result[tstr][tcount]["z"] = pos.z;
			team_count[t]++;
		}
		catch (const BadClientPlayer& e) {
			(void)e;
			// no player in this slot, check the next
		}
	}
}

void add_flags_to_result(HANDLE phandle, crow::json::wvalue& result) {
	lz::ClientCapturePointEntity_ClassInfo c(phandle, 0x143CCE7E0);
	try {
		auto flag = c.first();
		bool has_next = true;

		int flag_i = 0;
		while (has_next) {
			has_next = flag.hasNext();
			result["flags"][flag_i]["OwnerCapturePercent"] = flag.ownerCapturePercent();
			result["flags"][flag_i]["OwnerTeam"] = flag.ownerTeam();
			result["flags"][flag_i]["AttackingTeam"] = flag.attackingTeam();
			result["flags"][flag_i]["Defenders"] = flag.defenders();
			auto pos = flag.position();
			result["flags"][flag_i]["x"] = pos.x;
			result["flags"][flag_i]["y"] = pos.y;
			result["flags"][flag_i]["z"] = pos.z;
			flag_i++;
			if (has_next) flag = flag.next();
		}
	}
	catch (const NoCapturePointEntityExists& e) {
		(void)e;
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
			string local_name = read_local_player(player_manager, result);
			read_remote_players(player_manager, result, local_name);
			add_flags_to_result(phandle, result);
			return result;
		}
		catch (const BadClientPlayerManager& e) {
			(void)e;
			result["status"] = "failure";
			result["reason"] = "bad client player manager";
			return result;
		}
		catch (const BadClientGameManager& e) {
			(void)e;
			result["status"] = "failure";
			result["reason"] = "bad client game manager";
			return result;
		}

	});
	app.port(18080).multithreaded().run();
}

