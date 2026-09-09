//
// Created by octo on 26. 9. 9..
//

#include "RenderCycle.h"

#include <r3d/r3d.h>

#include "../Constants.h"
#include "../UnitManager.h"

namespace rdc {

    R3D_Model diceModel;
    R3D_Model tableModel;
    R3D_Model backModel;
    R3D_Model cupModel;


    void renderSetup() {

        diceModel =
            R3D_LoadModel(
                "../models/dice.glb"
            );

        tableModel =
            R3D_LoadModel(
                "../models/table.glb"
            );

        backModel =
            R3D_LoadModel(
                "../models/back.glb"
            );

        cupModel =
            R3D_LoadModel(
                "../models/cup.glb"
            );
    }


    static R3D_Model* getModel(
        con::Type type
    ) {

        switch (type) {

            case con::Type::DICE:

                return &diceModel;


            case con::Type::TABLE:

                return &tableModel;


            case con::Type::BACK:

                return &backModel;


            case con::Type::CUP:

                return &cupModel;


            case con::Type::SYSTEM:

                return nullptr;
        }


        return nullptr;
    }


    ctc::cycLog renderCycle(
        const Camera3D& camera
    ) {

        ctc::cycLog normalLog{
            4,
            0
        };


        R3D_Begin(
            camera
        );


        for (
            const auto& unit :
            umg::units
        ) {

            R3D_Model* model =
                getModel(
                    unit.type
                );


            if (
                model == nullptr
            )
                continue;


            Matrix transform =
                unit.getMatrix();


            R3D_DrawModelPro(
                *model,
                transform
            );
        }


        R3D_End();


        return normalLog;
    }

}