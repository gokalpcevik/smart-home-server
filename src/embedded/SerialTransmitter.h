#pragma once
#include <cstdint>
#include <string_view>

#include "../core/MinimalWindows.h"

namespace shm::embedded
{
	class SerialTransmitter
	{
	public:
		SerialTransmitter() = default;
		SerialTransmitter(std::wstring_view PortName, DCB const& SerialParams);


	private:

		HANDLE m_hComm{};

	};


}