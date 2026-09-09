//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_EVENTCYCLE_H
#define WIMY_EVENTCYCLE_H

#include <vector>

#include "raylib.h"
#include "CycleToCycle.h"

namespace evtc {

    constexpr int SYSTEM_ID = -1;


    enum class EvntType : int {

        NONE,

        // Local
        CLICK,
        HOST_ROOM,
        JOIN_ROOM,
        BACK_MENU,

        // Network / Game
        JOIN,

        GAME_START,

        ROLL,
        KEEP,

        SCORE_ONES,
        SCORE_TWOS,
        SCORE_THREES,
        SCORE_FOURS,
        SCORE_FIVES,
        SCORE_SIXES,

        SCORE_THREE_KIND,
        SCORE_FOUR_KIND,
        SCORE_FULL_HOUSE,

        SCORE_SMALL_STRAIGHT,
        SCORE_LARGE_STRAIGHT,

        SCORE_YAHTZEE,
        SCORE_CHANCE
    };


    struct Evnt {

        int id =
            SYSTEM_ID;

        EvntType type =
            EvntType::NONE;

        int port =
            0;


        // 이 기기에서만 처리
        bool local =
            false;


        Vector2 mouse{
            0.0f,
            0.0f
        };


        bool valid =
            true;
    };


    extern std::vector<Evnt>
        events;


    // Client -> Host 요청
    extern std::vector<Evnt>
        requests;


    ctc::cycLog eventCycle();

}

#endif // WIMY_EVENTCYCLE_H