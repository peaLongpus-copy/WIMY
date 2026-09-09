//
// Created by octo on 26. 9. 8..
//

#ifndef WIMY_BACKSYSTEM_H
#define WIMY_BACKSYSTEM_H

#include "raylib.h"
#include "r3d/r3d_core.h"

#include "UIManager.h"

#include "cycles/RenderCycle.h"
#include "cycles/StatusCycle.h"

namespace bst {

    constexpr int GAME_W =
        1920;

    constexpr int GAME_H =
        1080;


    int gameStart(
        auto loopFunc,
        const bool setUp
    ) {

        InitWindow(
            GAME_W,
            GAME_H,
            "WIMY"
        );


        SetTargetFPS(
            60
        );


        R3D_Init(
            GAME_W,
            GAME_H
        );


        if (setUp) {

            sttc::status =
                sttc::Status::MENU;


            rdc::renderSetup();


            uim::rebuild();
        }


        while (
            !WindowShouldClose()
        ) {

            loopFunc();
        }


        R3D_Close();

        CloseWindow();


        return 0;
    }

}

#endif // WIMY_BACKSYSTEM_H