#pragma once
#include <string>
#include <cstdint>

class Player
{
public:
	Player() {}
	Player(const std::string& name, int team) 
		: name(name), team(team), is_vehicle(false), health(0), max_health(0), fov(0), yaw(0), x(0), y(0), z(0) {}

	Player(const std::string& name, int team, bool is_vehicle, float health, float max_health, float x, float y, float z)
		: name(name), team(team), is_vehicle(is_vehicle), health(health), max_health(max_health), fov(0), yaw(0), x(x), y(y), z(z) {}

	Player(const std::string& name, int team, bool is_vehicle, float health, float max_health, float fov, float yaw, float x, float y, float z)
		: name(name), team(team), is_vehicle(is_vehicle), health(health), max_health(max_health), fov(fov), yaw(yaw), x(x), y(y), z(z) {}

	std::string name;
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

