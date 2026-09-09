//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_RENDERCYCLE_H
#define WIMY_RENDERCYCLE_H

#include "CycleToCycle.h"
#include "raylib.h"

namespace rdc {

    void renderSetup();


    ctc::cycLog renderCycle(
        const Camera3D& camera
    );

}

#endif // WIMY_RENDERCYCLE_H