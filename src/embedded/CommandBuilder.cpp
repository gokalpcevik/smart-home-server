#include "CommandBuilder.h"

namespace shm::embedded
{
	
	uint64_t CommandBuilder::BuildDoor(bool on)
	{
		return static_cast<uint64_t>(COMMAND::Door) | ((uint64_t)on << 8);
	}

	uint64_t CommandBuilder::BuildWindow(bool on)
	{
		return static_cast<uint64_t>(COMMAND::Window) | ((uint64_t)on << 8);
	}

	uint64_t CommandBuilder::BuildPower(ROOM room, bool on)
	{
		// 8 + 8 + 32 = 48 bytes total
		// data[0:7] is the command id
		// data[7:15] is the room id
		// data[22:15] is the brightness value

		return static_cast<uint64_t>(COMMAND::Power) |
			static_cast<uint64_t>(room) << 8 |
			static_cast<uint64_t>(on)   << 16;
	}

	uint64_t CommandBuilder::BuildBrightness(ROOM room, uint32_t brightness)
	{
		// 8 + 8 + 32 = 48 bytes total
		// data[0:7] is the function id
		// data[7:15] is the room id
		// data[47:16] is the brightness value
		return
			static_cast<uint64_t>(COMMAND::Brightness) |
			static_cast<uint64_t>(room)          << 8 |
			static_cast<uint64_t>(brightness)    << 16;
	}

	uint64_t CommandBuilder::BuildSelectColor(COLOR color)
	{
		return
			static_cast<uint64_t>(COMMAND::ColorSelection) |
			static_cast<uint64_t>(color) << 8;
	}

	uint64_t CommandBuilder::BuildSmartLight(bool on)
	{
		return static_cast<uint64_t>(COMMAND::Smartlight) | (static_cast<uint64_t>(on) << 8);
	}

	uint64_t CommandBuilder::BuildSunlight(bool on)
	{
		return static_cast<uint64_t>(COMMAND::Sunlight) | static_cast<uint64_t>(on) << 8;
	}
}
