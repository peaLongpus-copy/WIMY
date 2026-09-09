//
// Created by octo on 26. 9. 9..
//

#include "PhysicsCycle.h"

#include <cmath>

#include "raylib.h"
#include "raymath.h"

#include "../UnitManager.h"

#include "StatusCycle.h"

namespace phyc {

    constexpr float GRAVITY =
        -9.81f;

    constexpr float DICE_RADIUS =
        0.5f;

    constexpr float RESTITUTION =
        0.45f;


    static void groundCollision(
        con::unit& unit
    ) {

        float bottom =
            unit.transform.position.y -
            DICE_RADIUS;


        if (
            bottom >= 0.0f
        )
            return;


        unit.transform.position.y =
            DICE_RADIUS;


        if (
            unit.velocity.y <
            0.0f
        ) {

            unit.velocity.y =
                -unit.velocity.y *
                RESTITUTION;
        }


        unit.velocity.x *=
            0.92f;

        unit.velocity.z *=
            0.92f;


        if (
            fabsf(
                unit.velocity.y
            ) < 0.08f
        ) {

            unit.velocity.y =
                0.0f;
        }


        if (
            fabsf(
                unit.velocity.x
            ) < 0.02f
        ) {

            unit.velocity.x =
                0.0f;
        }


        if (
            fabsf(
                unit.velocity.z
            ) < 0.02f
        ) {

            unit.velocity.z =
                0.0f;
        }
    }


    static void diceCollision(
        con::unit& a,
        con::unit& b
    ) {

        Vector3 delta =
            Vector3Subtract(
                b.transform.position,
                a.transform.position
            );


        float distance =
            Vector3Length(
                delta
            );


        float minimumDistance =
            DICE_RADIUS *
            2.0f;


        if (
            distance >=
                minimumDistance
        )
            return;


        if (
            distance <=
            0.0001f
        ) {

            delta =
                {1, 0, 0};

            distance =
                1.0f;
        }


        Vector3 normal =
            Vector3Scale(
                delta,
                1.0f / distance
            );


        float penetration =
            minimumDistance -
            distance;


        a.transform.position =
            Vector3Subtract(
                a.transform.position,

                Vector3Scale(
                    normal,
                    penetration *
                    0.5f
                )
            );


        b.transform.position =
            Vector3Add(
                b.transform.position,

                Vector3Scale(
                    normal,
                    penetration *
                    0.5f
                )
            );


        Vector3 relativeVelocity =
            Vector3Subtract(
                b.velocity,
                a.velocity
            );


        float velocityNormal =
            Vector3DotProduct(
                relativeVelocity,
                normal
            );


        if (
            velocityNormal > 0.0f
        )
            return;


        float impulseMagnitude =
            -(
                1.0f +
                RESTITUTION
            )
            *
            velocityNormal
            /
            2.0f;


        Vector3 impulse =
            Vector3Scale(
                normal,
                impulseMagnitude
            );


        a.velocity =
            Vector3Subtract(
                a.velocity,
                impulse
            );


        b.velocity =
            Vector3Add(
                b.velocity,
                impulse
            );
    }


    ctc::cycLog physicsCycle() {

        ctc::cycLog normalLog{
            2,
            0
        };


        /*
         * Physics authority = Host.
         */
        if (
            !sttc::host
        )
            return normalLog;


        if (
            sttc::status !=
                sttc::Status::PLAYING
            &&
            sttc::status !=
                sttc::Status::ENDING
        )
            return normalLog;


        float deltaTime =
            GetFrameTime();


        for (
            auto& unit :
            umg::units
        ) {

            if (
                unit.type !=
                    con::Type::DICE
                ||
                unit.mass <=
                    0.0f
            )
                continue;


            unit.velocity.y +=
                GRAVITY *
                deltaTime;


            unit.transform.position =
                Vector3Add(
                    unit.transform.position,

                    Vector3Scale(
                        unit.velocity,
                        deltaTime
                    )
                );


            groundCollision(
                unit
            );
        }


        for (
            size_t i = 0;
            i < umg::units.size();
            ++i
        ) {

            if (
                umg::units[i].type !=
                con::Type::DICE
            )
                continue;


            for (
                size_t j = i + 1;
                j < umg::units.size();
                ++j
            ) {

                if (
                    umg::units[j].type !=
                    con::Type::DICE
                )
                    continue;


                diceCollision(
                    umg::units[i],
                    umg::units[j]
                );
            }
        }


        return normalLog;
    }

}