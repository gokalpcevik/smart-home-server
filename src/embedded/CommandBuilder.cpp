#include "CommandBuilder.h"

namespace shm::embedded
{
	uint64_t CommandBuilder::BuildDoorCmd(bool on)
	{
		return static_cast<uint64_t>(Command::Door) | ((uint64_t)on << 8);
	}

	uint64_t CommandBuilder::BuildWindowCmd(bool on)
	{
		return static_cast<uint64_t>(Command::Window) | ((uint64_t)on << 8);
	}

	uint64_t CommandBuilder::BuildPowerCmd(Room room, bool on)
	{
		// 8 + 8 + 32 = 48 bytes total
		// data[0:7] is the command id
		// data[7:15] is the room id
		// data[22:15] is the brightness value

		return static_cast<uint64_t>(Command::Power) |
			static_cast<uint64_t>(room) << 8 |
			static_cast<uint64_t>(on)   << 16;
	}

	uint64_t CommandBuilder::BuildBrightnessCmd(Room room, float brightness)
	{
		uint32_t brightnessU32 = (uint32_t)(brightness);

		// 8 + 8 + 32 = 48 bytes total
		// data[0:7] is the function id
		// data[7:15] is the room id
		// data[47:25] is the brightness value

		return
			static_cast<uint64_t>(Command::Brightness) |
			static_cast<uint64_t>(room)          << 8 |
			static_cast<uint64_t>(brightnessU32) << 16;
	}

	uint64_t CommandBuilder::BuildSelectColorCmd(Room room, Color color)
	{
		return
			static_cast<uint64_t>(Command::ColorSelection) |
			static_cast<uint64_t>(room)  << 8|
			static_cast<uint64_t>(color) << 16;
	}
}
