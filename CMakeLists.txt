
cmake_minimum_required(VERSION 3.10)

project(ParkingSimulator VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG_MODE=1")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DDEBUG_MODE=0")

find_package(Threads REQUIRED)

add_library(common STATIC
    common/protocol.c
    common/utils.c
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(common PUBLIC
    Threads::Threads
)

add_executable(server
    server/main.c
    server/server.c
    server/simulation.c
    server/parking.c
    server/vehicle.c
    server/statistics.c
    server/client_handler.c
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(server PRIVATE
    common
    Threads::Threads
)

set_target_properties(server PROPERTIES
    OUTPUT_NAME parking_server
)

add_executable(client
    client/main.c
    client/client.c
    client/network.c
    client/menu.c
    client/display.c
)

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(client PRIVATE
    common
    Threads::Threads
)

set_target_properties(client PROPERTIES
    OUTPUT_NAME parking_client
)

add_custom_target(all_build ALL
    DEPENDS server client
    COMMENT "Zostavujem server aj klient..."
)

install(TARGETS server client
    RUNTIME DESTINATION bin
)

message(STATUS "")
message(STATUS "========================================")
message(STATUS "Parking Simulator - CMake Configuration")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "")
message(STATUS "Dostupné ciele:")
message(STATUS "  make all     - Zostaví server aj klient")
message(STATUS "  make server  - Zostaví iba server")
message(STATUS "  make client  - Zostaví iba klient")
message(STATUS "  make clean   - Vymaže zostavené súbory")
message(STATUS "========================================")
message(STATUS "")
