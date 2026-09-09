//
// Created by octo on 26. 9. 9..
//

#include "UIManager.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

#include "ConnectSystem.h"
#include "UnitManager.h"

#include "cycles/StatusCycle.h"

namespace uim {

    std::vector<ui>
        uiLst;


    static int nextUiId =
        0;


    int uiMake(
        const ui& data
    ) {

        ui newUi =
            data;


        newUi.id =
            nextUiId++;


        uiLst.push_back(
            std::move(newUi)
        );


        return
            uiLst.back().id;
    }


    void uiRemove(
        int id
    ) {

        for (
            size_t i = 0;
            i < uiLst.size();
            ++i
        ) {

            if (
                uiLst[i].id != id
            )
                continue;


            uiLst[i] =
                std::move(
                    uiLst.back()
                );


            uiLst.pop_back();

            return;
        }
    }


    ui* uiSearch(
        int id
    ) {

        for (
            auto& data :
            uiLst
        ) {

            if (
                data.id == id
            )
                return &data;
        }


        return nullptr;
    }


    void rect(
        Vector2 position,
        Vector2 size,
        bool centered,
        float angle,
        Color color
    ) {

        Rectangle rec{

            position.x,
            position.y,

            size.x,
            size.y
        };


        Vector2 origin{
            0,
            0
        };


        if (centered) {

            origin = {

                size.x * 0.5f,
                size.y * 0.5f
            };
        }


        DrawRectanglePro(
            rec,
            origin,
            angle,
            color
        );
    }


    void rectLine(
        Vector2 position,
        Vector2 size,
        bool centered,
        float angle,
        float thickness,
        Color color
    ) {

        Vector2 origin{
            0,
            0
        };


        if (centered) {

            origin = {

                size.x * 0.5f,
                size.y * 0.5f
            };
        }


        Vector2 p1{
            -origin.x,
            -origin.y
        };

        Vector2 p2{
            size.x - origin.x,
            -origin.y
        };

        Vector2 p3{
            size.x - origin.x,
            size.y - origin.y
        };

        Vector2 p4{
            -origin.x,
            size.y - origin.y
        };


        float rad =
            angle *
            DEG2RAD;


        float cs =
            cosf(rad);

        float sn =
            sinf(rad);


        auto rotate =
            [
                position,
                cs,
                sn
            ]
            (
                Vector2 point
            ) {

                return Vector2{

                    position.x +
                    point.x * cs -
                    point.y * sn,

                    position.y +
                    point.x * sn +
                    point.y * cs
                };
            };


        p1 = rotate(p1);
        p2 = rotate(p2);
        p3 = rotate(p3);
        p4 = rotate(p4);


        DrawLineEx(
            p1,
            p2,
            thickness,
            color
        );

        DrawLineEx(
            p2,
            p3,
            thickness,
            color
        );

        DrawLineEx(
            p3,
            p4,
            thickness,
            color
        );

        DrawLineEx(
            p4,
            p1,
            thickness,
            color
        );
    }


    void circle(
        Vector2 position,
        float radius,
        bool centered,
        float angle,
        Color color
    ) {

        (void)centered;
        (void)angle;


        DrawCircleV(
            position,
            radius,
            color
        );
    }


    void circleLine(
        Vector2 position,
        float radius,
        bool centered,
        float angle,
        float thickness,
        Color color
    ) {

        (void)centered;
        (void)angle;


        float inner =
            std::max(
                0.0f,
                radius - thickness
            );


        DrawRing(
            position,

            inner,
            radius,

            0,
            360,

            64,

            color
        );
    }


    void text(
        const std::string& value,
        Vector2 position,
        float fontSize,
        bool centered,
        float angle,
        Color color
    ) {

        Font font =
            GetFontDefault();


        Vector2 textSize =
            MeasureTextEx(
                font,
                value.c_str(),
                fontSize,
                1.0f
            );


        Vector2 origin{
            0,
            0
        };


        if (centered) {

            origin = {

                textSize.x * 0.5f,
                textSize.y * 0.5f
            };
        }


        DrawTextPro(
            font,

            value.c_str(),

            position,

            origin,

            angle,

            fontSize,

            1.0f,

            color
        );
    }


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
    ) {

        ui data{};


        data.type =
            uiType::BUTTON;


        data.position =
            position;

        data.size =
            {
                width,
                height
            };


        data.centered =
            centered;

        data.border =
            border;


        data.color =
            color;

        data.borderColor =
            borderColor;


        data.value =
            value;

        data.textColor =
            textColor;


        data.event =
            event;


        return uiMake(
            data
        );
    }


    void drawButton(
        const ui& data
    ) {

        rect(
            data.position,
            data.size,
            data.centered,
            data.angle,
            data.color
        );


        if (
            data.border
        ) {

            rectLine(
                data.position,
                data.size,
                data.centered,
                data.angle,
                data.thickness,
                data.borderColor
            );
        }


        Vector2 textPosition =
            data.position;


        if (
            !data.centered
        ) {

            textPosition.x +=
                data.size.x *
                0.5f;

            textPosition.y +=
                data.size.y *
                0.5f;
        }


        text(
            data.value,
            textPosition,
            data.fontSize,
            true,
            data.angle,
            data.textColor
        );
    }


    static void addText(
        const std::string& value,
        Vector2 position,
        float size,
        bool centered = false,
        Color color = WHITE
    ) {

        ui data{};

        data.type =
            uiType::TEXT;

        data.value =
            value;

        data.position =
            position;

        data.fontSize =
            size;

        data.centered =
            centered;

        data.color =
            color;


        uiMake(
            data
        );
    }


    static evtc::Evnt scoreEvent(
        sttc::ScoreType type
    ) {

        evtc::Evnt event{};

        event.local =
            false;


        switch (type) {

            case sttc::ScoreType::ONES:
                event.type =
                    evtc::EvntType::SCORE_ONES;
                break;

            case sttc::ScoreType::TWOS:
                event.type =
                    evtc::EvntType::SCORE_TWOS;
                break;

            case sttc::ScoreType::THREES:
                event.type =
                    evtc::EvntType::SCORE_THREES;
                break;

            case sttc::ScoreType::FOURS:
                event.type =
                    evtc::EvntType::SCORE_FOURS;
                break;

            case sttc::ScoreType::FIVES:
                event.type =
                    evtc::EvntType::SCORE_FIVES;
                break;

            case sttc::ScoreType::SIXES:
                event.type =
                    evtc::EvntType::SCORE_SIXES;
                break;

            case sttc::ScoreType::THREE_KIND:
                event.type =
                    evtc::EvntType::SCORE_THREE_KIND;
                break;

            case sttc::ScoreType::FOUR_KIND:
                event.type =
                    evtc::EvntType::SCORE_FOUR_KIND;
                break;

            case sttc::ScoreType::FULL_HOUSE:
                event.type =
                    evtc::EvntType::SCORE_FULL_HOUSE;
                break;

            case sttc::ScoreType::SMALL_STRAIGHT:
                event.type =
                    evtc::EvntType::SCORE_SMALL_STRAIGHT;
                break;

            case sttc::ScoreType::LARGE_STRAIGHT:
                event.type =
                    evtc::EvntType::SCORE_LARGE_STRAIGHT;
                break;

            case sttc::ScoreType::YAHTZEE:
                event.type =
                    evtc::EvntType::SCORE_YAHTZEE;
                break;

            case sttc::ScoreType::CHANCE:
                event.type =
                    evtc::EvntType::SCORE_CHANCE;
                break;

            case sttc::ScoreType::COUNT:
                event.type =
                    evtc::EvntType::NONE;
                break;
        }


        return event;
    }


    static void buildMenu() {

        float centerX =
            GetScreenWidth() *
            0.5f;

        float centerY =
            GetScreenHeight() *
            0.5f;


        addText(
            "WIMY YAHTZEE",

            {
                centerX,
                centerY - 180
            },

            54,

            true
        );


        button(
            360,
            80,

            {
                centerX,
                centerY - 40
            },

            true,
            true,

            DARKGRAY,
            WHITE,

            "HOST GAME",
            WHITE,

            evtc::Evnt{
                .type =
                    evtc::EvntType::HOST_ROOM,

                .local =
                    true
            }
        );


        button(
            360,
            80,

            {
                centerX,
                centerY + 70
            },

            true,
            true,

            DARKGRAY,
            WHITE,

            "JOIN LOCAL",
            WHITE,

            evtc::Evnt{
                .type =
                    evtc::EvntType::JOIN_ROOM,

                .local =
                    true
            }
        );


        addText(
            "127.0.0.1:7777",

            {
                centerX,
                centerY + 150
            },

            22,

            true,

            GRAY
        );
    }


    static void buildRoom() {

        float centerX =
            GetScreenWidth() *
            0.5f;


        addText(
            sttc::host
                ? "HOST ROOM"
                : "JOINED ROOM",

            {
                centerX,
                120
            },

            46,

            true
        );


        addText(
            "127.0.0.1:7777",

            {
                centerX,
                170
            },

            22,

            true,

            GRAY
        );


        addText(
            "PLAYERS",

            {
                centerX,
                260
            },

            32,

            true
        );


        for (
            size_t i = 0;
            i < sttc::players.size();
            ++i
        ) {

            const auto& player =
                sttc::players[i];


            std::string line =
                "P" +
                std::to_string(
                    i + 1
                )
                +
                "   PORT "
                +
                std::to_string(
                    player.port
                );


            if (
                player.port ==
                consys::getLocalPort()
            ) {

                line +=
                    "   YOU";
            }


            addText(
                line,

                {
                    centerX,
                    320.0f +
                    static_cast<float>(i) *
                    45.0f
                },

                26,

                true
            );
        }


        if (
            sttc::host
        ) {

            button(
                300,
                75,

                {
                    centerX,
                    600
                },

                true,
                true,

                DARKGREEN,
                WHITE,

                "START",
                WHITE,

                evtc::Evnt{
                    .type =
                        evtc::EvntType::GAME_START,

                    .local =
                        false
                }
            );
        }
        else {

            addText(
                "WAITING FOR HOST",

                {
                    centerX,
                    600
                },

                28,

                true,

                LIGHTGRAY
            );
        }


        button(
            240,
            60,

            {
                centerX,
                730
            },

            true,
            true,

            MAROON,
            WHITE,

            "BACK",
            WHITE,

            evtc::Evnt{
                .type =
                    evtc::EvntType::BACK_MENU,

                .local =
                    true
            }
        );
    }


    static void buildGame() {

        int screenWidth =
            GetScreenWidth();

        int screenHeight =
            GetScreenHeight();


        int localPort =
            consys::getLocalPort();


        bool localTurn =
            sttc::currentPlayerPort()
            ==
            localPort;


        std::string top =
            "ROUND "
            +
            std::to_string(
                sttc::round
            )
            +
            "   ROLL "
            +
            std::to_string(
                sttc::rollCount
            )
            +
            "/3";


        if (
            !sttc::players.empty()
            &&
            sttc::currentPlayer >= 0
            &&
            sttc::currentPlayer <
                static_cast<int>(
                    sttc::players.size()
                )
        ) {

            top +=
                "   TURN P"
                +
                std::to_string(
                    sttc::currentPlayer + 1
                );
        }


        addText(
            top,

            {
                screenWidth *
                    0.5f,
                35
            },

            30,

            true
        );


        int playerCount =
            std::max(
                1,
                static_cast<int>(
                    sttc::players.size()
                )
            );


        float columnWidth =
            std::min(
                400.0f,

                (
                    screenWidth -
                    60.0f
                )
                /
                playerCount
            );


        float totalWidth =
            columnWidth *
            playerCount;


        float startX =
            (
                screenWidth -
                totalWidth
            )
            *
            0.5f;


        float startY =
            90.0f;

        float rowHeight =
            31.0f;


        for (
            int p = 0;
            p < static_cast<int>(
                sttc::players.size()
            );
            ++p
        ) {

            const auto& player =
                sttc::players[p];


            float x =
                startX +
                p *
                columnWidth;


            std::string title =
                "PLAYER "
                +
                std::to_string(
                    p + 1
                );


            if (
                player.port ==
                localPort
            ) {

                title +=
                    " (YOU)";
            }


            addText(
                title,

                {
                    x +
                    columnWidth *
                    0.5f,

                    startY
                },

                24,

                true,

                p ==
                    sttc::currentPlayer
                    ? YELLOW
                    : WHITE
            );


            for (
                int scoreIndex = 0;
                scoreIndex <
                    sttc::SCORE_COUNT;
                ++scoreIndex
            ) {

                auto scoreType =
                    static_cast<
                        sttc::ScoreType
                    >(
                        scoreIndex
                    );


                float y =
                    startY +
                    35.0f +
                    rowHeight *
                    scoreIndex;


                std::string label =
                    sttc::scoreName(
                        scoreType
                    );


                if (
                    player.used[
                        scoreIndex
                    ]
                ) {

                    label +=
                        "   "
                        +
                        std::to_string(
                            player.score[
                                scoreIndex
                            ]
                        );


                    ui background{};

                    background.type =
                        uiType::RECT;

                    background.position =
                        {
                            x + 4,
                            y
                        };

                    background.size =
                        {
                            columnWidth - 8,
                            rowHeight - 2
                        };

                    background.color =
                        Color{
                            45,
                            45,
                            45,
                            220
                        };


                    uiMake(
                        background
                    );


                    addText(
                        label,

                        {
                            x + 12,
                            y + 4
                        },

                        18
                    );
                }
                else {

                    bool canSelect =
                        p ==
                            sttc::currentPlayer
                        &&
                        localTurn
                        &&
                        sttc::rollCount > 0;


                    if (
                        p ==
                            sttc::currentPlayer
                        &&
                        sttc::rollCount > 0
                    ) {

                        label +=
                            "   ["
                            +
                            std::to_string(
                                sttc::calculateScore(
                                    scoreType
                                )
                            )
                            +
                            "]";
                    }


                    if (
                        canSelect
                    ) {

                        button(
                            columnWidth - 8,
                            rowHeight - 2,

                            {
                                x + 4,
                                y
                            },

                            false,
                            true,

                            Color{
                                35,
                                70,
                                50,
                                220
                            },

                            DARKGREEN,

                            label,
                            WHITE,

                            scoreEvent(
                                scoreType
                            )
                        );
                    }
                    else {

                        ui background{};

                        background.type =
                            uiType::RECT;

                        background.position =
                            {
                                x + 4,
                                y
                            };

                        background.size =
                            {
                                columnWidth - 8,
                                rowHeight - 2
                            };

                        background.color =
                            Color{
                                25,
                                25,
                                25,
                                205
                            };


                        uiMake(
                            background
                        );


                        addText(
                            label,

                            {
                                x + 12,
                                y + 4
                            },

                            18,

                            false,

                            GRAY
                        );
                    }
                }
            }


            float infoY =
                startY +
                35.0f +
                rowHeight *
                    sttc::SCORE_COUNT;


            addText(
                "Upper: "
                +
                std::to_string(
                    sttc::upperScore(p)
                ),

                {
                    x + 10,
                    infoY + 8
                },

                18
            );


            addText(
                "Bonus: "
                +
                std::string(
                    sttc::upperScore(p) >= 63
                    ? "35"
                    : "0"
                ),

                {
                    x + 10,
                    infoY + 32
                },

                18
            );


            addText(
                "TOTAL: "
                +
                std::to_string(
                    sttc::totalScore(p)
                ),

                {
                    x + 10,
                    infoY + 58
                },

                22,

                false,

                YELLOW
            );
        }


        float diceY =
            screenHeight -
            150.0f;


        int diceNumber =
            1;


        for (
            const auto& unit :
            umg::units
        ) {

            if (
                unit.type !=
                con::Type::DICE
            )
                continue;


            std::string value =
                "D"
                +
                std::to_string(
                    diceNumber
                )
                +
                ": "
                +
                std::to_string(
                    unit.value
                );


            if (
                unit.kept
            ) {

                value +=
                    " KEEP";
            }


            addText(
                value,

                {
                    80.0f +
                    (
                        diceNumber - 1
                    ) *
                    145.0f,

                    diceY
                },

                22,

                false,

                unit.kept
                    ? YELLOW
                    : WHITE
            );


            ++diceNumber;
        }


        if (
            localTurn
            &&
            sttc::rollCount < 3
        ) {

            button(
                260,
                70,

                {
                    screenWidth -
                    180.0f,

                    screenHeight -
                    100.0f
                },

                true,
                true,

                DARKBLUE,
                WHITE,

                sttc::rollCount == 0
                    ? "ROLL"
                    : "REROLL",

                WHITE,

                evtc::Evnt{
                    .type =
                        evtc::EvntType::ROLL,

                    .local =
                        false
                }
            );
        }
    }


    static void buildEnding() {

        float centerX =
            GetScreenWidth() *
            0.5f;


        addText(
            "GAME OVER",

            {
                centerX,
                100
            },

            52,

            true
        );


        int winnerIndex =
            sttc::playerSearch(
                sttc::winnerPort
            );


        if (
            winnerIndex != -1
        ) {

            addText(
                "WINNER: PLAYER "
                +
                std::to_string(
                    winnerIndex + 1
                ),

                {
                    centerX,
                    180
                },

                36,

                true,

                YELLOW
            );
        }


        for (
            size_t i = 0;
            i < sttc::players.size();
            ++i
        ) {

            addText(
                "PLAYER "
                +
                std::to_string(
                    i + 1
                )
                +
                "   "
                +
                std::to_string(
                    sttc::totalScore(
                        static_cast<int>(i)
                    )
                ),

                {
                    centerX,
                    280.0f +
                    static_cast<float>(i) *
                    55.0f
                },

                28,

                true
            );
        }


        button(
            300,
            70,

            {
                centerX,
                650
            },

            true,
            true,

            MAROON,
            WHITE,

            "BACK TO MENU",
            WHITE,

            evtc::Evnt{
                .type =
                    evtc::EvntType::BACK_MENU,

                .local =
                    true
            }
        );
    }


    void rebuild() {

        uiLst.clear();

        nextUiId =
            0;


        switch (
            sttc::status
        ) {

            case sttc::Status::NONE:
            case sttc::Status::MENU:

                buildMenu();

                break;


            case sttc::Status::ROOM:
            case sttc::Status::STARTING:

                buildRoom();

                break;


            case sttc::Status::PLAYING:

                buildGame();

                break;


            case sttc::Status::ENDING:

                buildEnding();

                break;
        }
    }

}