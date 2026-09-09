//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_UNITMANAGER_H
#define WIMY_UNITMANAGER_H

#include <array>
#include <vector>

#include "Constants.h"

namespace umg {

    extern std::vector<con::unit> units;


    int unitMake(
        con::Type type,
        float mass,
        con::Transform transform,
        Vector3 velocity = {0, 0, 0},
        con::SystemType systemType =
            con::SystemType::NONE,
        int ownerPort = 0,
        int value = 0,
        bool kept = false,
        std::array<int, 16> data = {}
    );


    void unitServe(
        int id,
        con::Type type,
        float mass,
        con::Transform transform,
        Vector3 velocity = {0, 0, 0},
        con::SystemType systemType =
            con::SystemType::NONE,
        int ownerPort = 0,
        int value = 0,
        bool kept = false,
        std::array<int, 16> data = {}
    );


    void unitRemove(
        int id
    );


    void unitSet(
        int id,
        con::Transform transform,
        Vector3 velocity
    );


    con::unit* unitSearch(
        int id
    );


    con::unit* systemSearch(
        con::SystemType type,
        int ownerPort = -1
    );


    void unitClear();

}

#endif // WIMY_UNITMANAGER_H