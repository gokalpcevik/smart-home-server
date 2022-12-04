#include "core/Application.h"

int
main(int argc, char* argv[])
{
    return shm::Core::Application::Get().Start();
}
