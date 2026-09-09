//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_UIMANAGER_H
#define WIMY_UIMANAGER_H

#include <string>
#include <vector>

#include "raylib.h"

#include "cycles/EventCycle.h"

namespace uim {

    enum class uiType {

        RECT,
        RECT_LINE,

        CIRCLE,
        CIRCLE_LINE,

        TEXT,

        BUTTON
    };


    struct ui {

        int id =
            -1;

        uiType type =
            uiType::RECT;


        Vector2 position{
            0,
            0
        };

        Vector2 size{
            0,
            0
        };


        float radius =
            0.0f;


        bool centered =
            false;

        float angle =
            0.0f;


        bool border =
            false;

        float thickness =
            2.0f;


        Color color =
            WHITE;

        Color borderColor =
            WHITE;


        std::string value;

        Color textColor =
            WHITE;

        float fontSize =
            24.0f;


        evtc::Evnt event{};
    };


    extern std::vector<ui>
        uiLst;


    int uiMake(
        const ui& data
    );


    void uiRemove(
        int id
    );


    ui* uiSearch(
        int id
    );


    void rect(
        Vector2 position,
        Vector2 size,
        bool centered,
        float angle,
        Color color
    );


    void rectLine(
        Vector2 position,
        Vector2 size,
        bool centered,
        float angle,
        float thickness,
        Color color
    );


    void circle(
        Vector2 position,
        float radius,
        bool centered,
        float angle,
        Color color
    );


    void circleLine(
        Vector2 position,
        float radius,
        bool centered,
        float angle,
        float thickness,
        Color color
    );


    void text(
        const std::string& value,
        Vector2 position,
        float fontSize,
        bool centered,
        float angle,
        Color color
    );


    int button(
        float width,
        float height,

        Vector2 position,

        bool centered,
        bool border,

        Color color,
        Color borderColor,

        const std::string& value,
        Color textColor,

        const evtc::Evnt& event
    );


    void drawButton(
        const ui& data
    );


    // 현재 Status에 맞게 UI 재구성
    void rebuild();

}

#endif // WIMY_UIMANAGER_H