//
// Created by octo on 26. 9. 8..
//

#ifndef WIMY_CONSTANTS_H
#define WIMY_CONSTANTS_H

#include <array>

#include "raylib.h"
#include "raymath.h"

namespace con {

    struct Transform {

        Vector3 position{
            0.0f,
            0.0f,
            0.0f
        };

        Quaternion rotation{
            0.0f,
            0.0f,
            0.0f,
            1.0f
        };

        Vector3 scale{
            1.0f,
            1.0f,
            1.0f
        };
    };


    enum class Type : int {

        DICE,

        TABLE,
        BACK,
        CUP,

        SYSTEM
    };


    enum class SystemType : int {

        NONE,

        PLAYER,
        CAMERA,
        GAME
    };


    struct unit {

        int id = -1;

        Type type =
            Type::SYSTEM;

        SystemType systemType =
            SystemType::NONE;

        int ownerPort = 0;


        float mass = 0.0f;


        Transform transform{};

        Vector3 velocity{
            0.0f,
            0.0f,
            0.0f
        };


        // DICE의 논리 값
        int value = 0;

        bool kept = false;


        /*
         * SYSTEM용 범용 데이터.
         *
         * PLAYER:
         * 0~12 = score
         * 13   = used bit mask
         * 14   = turn order
         * 15   = total score
         *
         * GAME:
         * 0 = current player
         * 1 = round
         * 2 = roll count
         * 3 = Status
         * 4 = winner port
         * 5 = player count
         */
        std::array<int, 16> data{};


        Matrix getMatrix() const {

            Matrix scaleMatrix =
                MatrixScale(
                    transform.scale.x,
                    transform.scale.y,
                    transform.scale.z
                );


            Matrix rotationMatrix =
                QuaternionToMatrix(
                    transform.rotation
                );


            Matrix translationMatrix =
                MatrixTranslate(
                    transform.position.x,
                    transform.position.y,
                    transform.position.z
                );


            return MatrixMultiply(
                MatrixMultiply(
                    scaleMatrix,
                    rotationMatrix
                ),
                translationMatrix
            );
        }
    };


    inline int nextId = 0;


    inline int makeId() {

        return nextId++;
    }

}

#endif // WIMY_CONSTANTS_H