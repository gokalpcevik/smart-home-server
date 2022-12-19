# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)


project ("smart-home-server")

find_package(spdlog CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(simdjson CONFIG REQUIRED)

add_executable(smart-home-server "src/Main.cpp")

add_subdirectory ("src")


set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CXX_STANDARD 20
        )

message("Boost Libraries: ${Boost_LIBRARIES}")

include_directories(smart-home-server PRIVATE
    ${Boost_INCLUDE_DIRS})

target_link_libraries(smart-home-server 
    PRIVATE 
    spdlog::spdlog
    simdjson::simdjson
    )

target_link_directories(smart-home-server PRIVATE
    Boost::boost
    ${Boost_LIBRARIES})