#pragma once
#include <cstdint>
#include <cassert>

namespace shm::embedded
{
	enum class ROOM : uint64_t
	{
		Invalid=0x0,
		LivingRoom=0x1,
		Bedroom=0x2
	};

	enum class COMMAND : uint64_t
	{
		Brightness=0x0,
		Power=0x1,
		Smartlight=0x2,
		Sunlight=0x3,
		Window=0x4,
		Door=0x5,
		ColorSelection=0x6
	};

	enum class COLOR
	{
		Red = 0x0,
		Green = 0x1,
		Blue = 0x2,
		Cyan = 0x3
	};

	class CommandBuilder
	{
	public:
		CommandBuilder() = default;

		static uint64_t BuildInvalid() { return 0x0; }
		static uint64_t BuildDoor(bool on);
		static uint64_t BuildWindow(bool on);
		static uint64_t BuildPower(ROOM room, bool on);
		static uint64_t BuildBrightness(ROOM room, uint32_t brightness);
		static uint64_t BuildSelectColor(COLOR color);
		static uint64_t BuildSmartLight(bool on);
		static uint64_t BuildSunlight(bool on);
	};
}