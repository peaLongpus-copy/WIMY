//
// Created by octo on 26. 9. 9..
//

#include "UIRenderCycle.h"

#include "../UIManager.h"

namespace uirc {

    ctc::cycLog uiRenderCycle() {

        ctc::cycLog normalLog{
            7,
            0
        };


        for (
            const auto& data :
            uim::uiLst
        ) {

            switch (
                data.type
            ) {

                case uim::uiType::RECT: {

                    uim::rect(
                        data.position,
                        data.size,
                        data.centered,
                        data.angle,
                        data.color
                    );

                    break;
                }


                case uim::uiType::RECT_LINE: {

                    uim::rectLine(
                        data.position,
                        data.size,
                        data.centered,
                        data.angle,
                        data.thickness,
                        data.color
                    );

                    break;
                }


                case uim::uiType::CIRCLE: {

                    uim::circle(
                        data.position,
                        data.radius,
                        data.centered,
                        data.angle,
                        data.color
                    );

                    break;
                }


                case uim::uiType::CIRCLE_LINE: {

                    uim::circleLine(
                        data.position,
                        data.radius,
                        data.centered,
                        data.angle,
                        data.thickness,
                        data.color
                    );

                    break;
                }


                case uim::uiType::TEXT: {

                    uim::text(
                        data.value,
                        data.position,
                        data.fontSize,
                        data.centered,
                        data.angle,
                        data.color
                    );

                    break;
                }


                case uim::uiType::BUTTON: {

                    uim::drawButton(
                        data
                    );

                    break;
                }
            }
        }


        return normalLog;
    }

}