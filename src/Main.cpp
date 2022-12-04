
#include <SDKDDKVer.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT=0x0601;
#endif
#include "core/Application.h"

int
main(int argc, char* argv[])
{
    return shm::Core::Application::Get().Start();
}
