#pragma once
#include <cstdint>
#include <cassert>

namespace shm::embedded
{
	enum class Room : uint8_t
	{
		Invalid=0x0,
		LivingRoom=0x1,
		Bedroom=0x2
	};

	enum class Command : uint8_t
	{
		Brightness=0x0,
		Power=0x1,
		Smartlight=0x2,
		Sunlight=0x3,
		Window=0x4,
		Door=0x5,
		ColorSelection=0x6
	};

	enum class Color
	{
		Red=0x0,
		Green=0x1,
		Blue=0x2
	};

	class CommandBuilder
	{
	public:
		CommandBuilder() = default;

		static uint64_t BuildInvalidCmd() { return 0x0; }
		static uint64_t BuildDoorCmd(bool on);
		static uint64_t BuildWindowCmd(bool on);
		static uint64_t BuildPowerCmd(Room room, bool on);
		static uint64_t BuildBrightnessCmd(Room room, float brightness);
		static uint64_t BuildSelectColorCmd(Room room, Color color);
	};
}