#pragma once
#include <cstdint>
#include <string_view>

#include "../core/MinimalWindows.h"
#include "../core/Log.h"

namespace shm::embedded
{
	class SerialComm
	{
	public:
		SerialComm() = default;
		SerialComm(SerialComm const&) = delete;
		SerialComm(std::string_view PortName);
		~SerialComm();

		bool WriteString(std::string const& message) const;

	private:
		HANDLE m_HComm{INVALID_HANDLE_VALUE};
		DCB m_DCBSerialParams{};
		std::string_view m_PortName{};
	};


}