#pragma once
#include <string>
#include <cstdint>

struct Player
{
	std::string name;
	std::string weapon_name;
	int32_t team;
	bool is_vehicle;
	float health;
	float max_health;
	float fov;
	float yaw;
	float x;
	float y;
	float z;
};

