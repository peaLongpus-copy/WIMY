cmake_minimum_required(VERSION 4.2)

project(WIMY)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)


set(
        R3D_RAYLIB_VENDORED
        ON
        CACHE BOOL ""
        FORCE
)

set(
        R3D_ASSIMP_VENDORED
        ON
        CACHE BOOL ""
        FORCE
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        r3d

        GIT_REPOSITORY
        https://github.com/Bigfoot71/r3d.git

        GIT_TAG
        master

        GIT_SHALLOW
        TRUE

        GIT_SUBMODULES_RECURSE
        TRUE
)


FetchContent_MakeAvailable(
        r3d
)


add_executable(
        WIMY

        main.cpp

        BackSystem.cpp
        BackSystem.h

        Camera.cpp
        Camera.h

        Constants.h

        ConnectSystem.cpp
        ConnectSystem.h

        Listener.cpp
        Listener.h

        Packet.h

        UnitManager.cpp
        UnitManager.h

        UIManager.cpp
        UIManager.h


        cycles/CycleToCycle.cpp
        cycles/CycleToCycle.h

        cycles/StatusCycle.cpp
        cycles/StatusCycle.h

        cycles/EventCycle.cpp
        cycles/EventCycle.h

        cycles/SyncCycle.cpp
        cycles/SyncCycle.h

        cycles/PhysicsCycle.cpp
        cycles/PhysicsCycle.h

        cycles/DistributionCycle.cpp
        cycles/DistributionCycle.h

        cycles/RenderCycle.cpp
        cycles/RenderCycle.h

        cycles/UIRenderCycle.cpp
        cycles/UIRenderCycle.h
)


target_link_libraries(
        WIMY

        PRIVATE

        r3d
        raylib
)


target_include_directories(
        WIMY

        PRIVATE

        ${r3d_SOURCE_DIR}/include
        ${r3d_SOURCE_DIR}/external/raylib/src
)
if (WIN32)

    target_link_libraries(
            WIMY
            PRIVATE
            ws2_32
    )


    if (MINGW)

        target_link_options(
                WIMY
                PRIVATE

                -static
                -static-libgcc
                -static-libstdc++
        )

    endif()

endif()