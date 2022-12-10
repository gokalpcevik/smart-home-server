#include "SerialCommunication.h"

namespace shm::embedded
{
	SerialCommunication::SerialCommunication(std::string_view portName,BaudRate baudRate)
		: m_PortName(portName)
	{
		m_HComm = ::CreateFileA(portName.data(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr);

		if(m_HComm == INVALID_HANDLE_VALUE)
		{
			shm_error("Port with name:{0} cannot be opened.",portName);
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

		m_DCBSerialParams.BaudRate = static_cast<DWORD>(baudRate);
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
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		if (!::SetCommTimeouts(m_HComm, &timeouts))
		{
			shm_error("Error while setting communication timeouts.");
			return;
		}
	}

	SerialCommunication::~SerialCommunication()
	{
		if(m_HComm != INVALID_HANDLE_VALUE)
			::CloseHandle(m_HComm);
	}

	bool SerialCommunication::WriteString(std::string const& message) const
	{
		DWORD BytesWritten;
		return static_cast<bool>(::WriteFile(m_HComm, message.data(), message.size(), &BytesWritten, nullptr));
	}

	bool SerialCommunication::ReadBlocking(std::vector<char>& receivedData) const
	{
		BOOL status = ::SetCommMask(m_HComm, EV_RXCHAR);
		if(!status)
		{
			shm_error("Error while setting CommMask.");
			return false;
		}

		DWORD dwEventMask;
		status = ::WaitCommEvent(m_HComm, &dwEventMask, nullptr);

		if(!status)
		{
			shm_error("Error while waiting for CommEvent.");
			return false;
		}

		DWORD numBytesRead;
		char readData;

		do
		{
			status = ReadFile(m_HComm, &readData, sizeof(readData), &numBytesRead, nullptr);
			if(!status)
			{
				shm_error("Error while trying to read the serial file.");
				return false;
			} 
			receivedData.push_back(readData);

		} while (numBytesRead > 0);

		return true;
	}
}
