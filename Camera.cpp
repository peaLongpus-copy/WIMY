//
// Created by octo on 26. 9. 9..
//

#include "Camera.h"

namespace cmr {

    Camera3D camera{

        .position =
            {4.0f, 3.0f, 4.0f},

        .target =
            {0.0f, 0.0f, 0.0f},

        .up =
            {0.0f, 1.0f, 0.0f},

        .fovy =
            60.0f,

        .projection =
            CAMERA_PERSPECTIVE
    };

}