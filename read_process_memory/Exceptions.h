#pragma once

#include <stdexcept>

// These class support graceful response to when pointers aren't valid. The lazy readers
// assume things will work and will report failure via these exceptions. None of these are
// critical, shut-it-down errors. These occur mostly when the player is dead or not in a game.
class BadClientPlayer : public std::out_of_range
{
public:
	BadClientPlayer() : std::out_of_range("Bad ClientPlayer pointer") {}
};
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

class NoCapturePointEntityExists : public std::out_of_range
{
public:
	NoCapturePointEntityExists() : std::out_of_range("No Capture Point Entities are available") {}
};

