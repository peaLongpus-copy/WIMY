//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_PACKET_H
#define WIMY_PACKET_H

#include <array>
#include <type_traits>

#include "Constants.h"
#include "cycles/EventCycle.h"

namespace net {

    enum class DataType : int {

        EVENT,
        STATE
    };


    struct UnitState {

        int id = -1;

        con::Type type =
            con::Type::SYSTEM;

        con::SystemType systemType =
            con::SystemType::NONE;

        int ownerPort =
            0;


        float mass =
            0.0f;


        con::Transform transform{};

        Vector3 velocity{
            0.0f,
            0.0f,
            0.0f
        };


        int value =
            0;

        bool kept =
            false;


        std::array<int, 16>
            data{};
    };


    struct Packet {

        DataType type =
            DataType::EVENT;

        evtc::Evnt event{};

        UnitState state{};
    };


    static_assert(
        std::is_trivially_copyable_v<
            Packet
        >
    );

}

#endif // WIMY_PACKET_H