//
// Created by octo on 26. 9. 9..
//

#include "UnitManager.h"

#include <algorithm>
#include <utility>

namespace umg {

    std::vector<con::unit> units;


    int unitMake(
        con::Type type,
        float mass,
        con::Transform transform,
        Vector3 velocity,
        con::SystemType systemType,
        int ownerPort,
        int value,
        bool kept,
        std::array<int, 16> data
    ) {

        con::unit unit{};

        unit.id =
            con::makeId();

        unit.type =
            type;

        unit.systemType =
            systemType;

        unit.ownerPort =
            ownerPort;

        unit.mass =
            mass;

        unit.transform =
            transform;

        unit.velocity =
            velocity;

        unit.value =
            value;

        unit.kept =
            kept;

        unit.data =
            data;


        units.push_back(
            unit
        );


        return unit.id;
    }


    void unitServe(
        int id,
        con::Type type,
        float mass,
        con::Transform transform,
        Vector3 velocity,
        con::SystemType systemType,
        int ownerPort,
        int value,
        bool kept,
        std::array<int, 16> data
    ) {

        con::unit unit{};

        unit.id =
            id;

        unit.type =
            type;

        unit.systemType =
            systemType;

        unit.ownerPort =
            ownerPort;

        unit.mass =
            mass;

        unit.transform =
            transform;

        unit.velocity =
            velocity;

        unit.value =
            value;

        unit.kept =
            kept;

        unit.data =
            data;


        units.push_back(
            unit
        );


        con::nextId =
            std::max(
                con::nextId,
                id + 1
            );
    }


    void unitRemove(
        int id
    ) {

        for (
            size_t i = 0;
            i < units.size();
            ++i
        ) {

            if (
                units[i].id != id
            )
                continue;


            units[i] =
                std::move(
                    units.back()
                );


            units.pop_back();

            return;
        }
    }


    void unitSet(
        int id,
        con::Transform transform,
        Vector3 velocity
    ) {

        con::unit* unit =
            unitSearch(id);


        if (
            unit == nullptr
        )
            return;


        unit->transform =
            transform;

        unit->velocity =
            velocity;
    }


    con::unit* unitSearch(
        int id
    ) {

        for (
            auto& unit :
            units
        ) {

            if (
                unit.id == id
            )
                return &unit;
        }


        return nullptr;
    }


    con::unit* systemSearch(
        con::SystemType type,
        int ownerPort
    ) {

        for (
            auto& unit :
            units
        ) {

            if (
                unit.type !=
                con::Type::SYSTEM
            )
                continue;


            if (
                unit.systemType !=
                type
            )
                continue;


            if (
                ownerPort != -1
                &&
                unit.ownerPort !=
                    ownerPort
            )
                continue;


            return &unit;
        }


        return nullptr;
    }


    void unitClear() {

        units.clear();

        con::nextId =
            0;
    }

}