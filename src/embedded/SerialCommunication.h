#pragma once
#include <cstdint>
#include <string_view>

#include "../core/MinimalWindows.h"
#include "../core/Log.h"

namespace shm::embedded
{
	enum class BaudRate : uint32_t
	{
		BR9600    = CBR_9600,
		BR38400   = CBR_38400,
		BR115200  = CBR_115200
	};

	class SerialCommunication
	{
	public:
		SerialCommunication() = default;
		SerialCommunication(SerialCommunication const&) = delete;
		SerialCommunication& operator=(SerialCommunication const&) = delete;
		SerialCommunication(std::string_view portName, BaudRate baudRate);
		~SerialCommunication();

		bool WriteString(std::string const& message) const;
		bool ReadBlocking(std::vector<char>& receivedData) const;

	private:
		HANDLE m_HComm{INVALID_HANDLE_VALUE};
		DCB m_DCBSerialParams{};
		std::string_view m_PortName{};
	};


}