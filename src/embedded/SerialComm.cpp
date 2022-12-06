#include "SerialComm.h"

namespace shm::embedded
{
	SerialComm::SerialComm(std::string_view PortName)
		: m_PortName(PortName)
	{
		m_HComm = ::CreateFileA(PortName.data(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr);

		if(m_HComm == INVALID_HANDLE_VALUE)
		{
			shm_error("Port with name:{0} cannot be opened.",PortName);
			return;
		}

		m_DCBSerialParams.DCBlength = sizeof(DCB);

		bool CommState = ::GetCommState(m_HComm, &m_DCBSerialParams);

		if(!CommState)
		{
			// NOTE:Might change the flow of this later and just log the translated error into cerr.
			shm_error("Error getting communication state. Call GetLastError for more info.");
			return;
		}

		m_DCBSerialParams.BaudRate = CBR_9600;
		m_DCBSerialParams.ByteSize = 8U;
		m_DCBSerialParams.StopBits = 1U;
		m_DCBSerialParams.Parity = NOPARITY;

		CommState = ::SetCommState(m_HComm, &m_DCBSerialParams);

		if(!CommState)
		{
			// NOTE:Might change the flow of this later and just log the translated error into cerr.
			shm_error("Error setting communication state. Call GetLastError for more info.");
			return;
		}

		COMMTIMEOUTS timeouts{};
		timeouts.ReadIntervalTimeout = 100;
		timeouts.ReadTotalTimeoutConstant = 100;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 100;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		if (!::SetCommTimeouts(m_HComm, &timeouts))
		{
			shm_error("Error while setting communication timeouts.");
			return;
		}
	}

	SerialComm::~SerialComm()
	{
		if(m_HComm != INVALID_HANDLE_VALUE)
			::CloseHandle(m_HComm);
	}

	bool SerialComm::WriteString(std::string const& message) const
	{
		DWORD BytesWritten;
		return static_cast<bool>(::WriteFile(m_HComm, message.data(), message.size(), &BytesWritten, nullptr));
	}
}
