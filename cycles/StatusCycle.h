//
// Created by octo on 26. 9. 8..
//

#ifndef WIMY_STATUSCYCLE_H
#define WIMY_STATUSCYCLE_H

#include <array>
#include <vector>

#include "CycleToCycle.h"

namespace sttc {

    constexpr int MAX_PLAYERS =
        4;


    enum class Status : int {

        NONE,
        MENU,
        ROOM,
        STARTING,
        PLAYING,
        ENDING
    };


    enum class ScoreType : int {

        ONES,
        TWOS,
        THREES,
        FOURS,
        FIVES,
        SIXES,

        THREE_KIND,
        FOUR_KIND,
        FULL_HOUSE,

        SMALL_STRAIGHT,
        LARGE_STRAIGHT,

        YAHTZEE,
        CHANCE,

        COUNT
    };


    constexpr int SCORE_COUNT =
        static_cast<int>(
            ScoreType::COUNT
        );


    struct Player {

        int port =
            0;

        int unitId =
            -1;

        int order =
            -1;


        std::array<int, SCORE_COUNT>
            score{};


        std::array<bool, SCORE_COUNT>
            used{};
    };


    extern bool gameStart;

    extern bool host;


    extern Status status;


    extern std::vector<Player>
        players;


    extern int currentPlayer;

    extern int round;

    extern int rollCount;

    extern int winnerPort;


    int playerSearch(
        int port
    );


    int playerAdd(
        int port
    );


    int currentPlayerPort();


    int upperScore(
        int playerIndex
    );


    int totalScore(
        int playerIndex
    );


    bool gameFinished();


    int calculateScore(
        ScoreType type
    );


    const char* scoreName(
        ScoreType type
    );


    void resetSession();


    void syncStatusToSystemUnits();


    void syncStatusFromSystemUnits();


    ctc::cycLog statusCycle();

}

#endif // WIMY_STATUSCYCLE_H