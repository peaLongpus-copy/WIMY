//
// Created by octo on 26. 9. 9..
//

#include "EventCycle.h"

#include <algorithm>
#include <random>

#include "raymath.h"

#include "StatusCycle.h"

#include "../Camera.h"
#include "../ConnectSystem.h"
#include "../UIManager.h"
#include "../UnitManager.h"

namespace evtc {

    std::vector<Evnt>
        events;


    std::vector<Evnt>
        requests;


    static std::mt19937 rng{
        std::random_device{}()
    };


    static con::Transform
    defaultTransform() {

        return con::Transform{
            .position =
                {0, 0, 0},

            .rotation =
                {0, 0, 0, 1},

            .scale =
                {1, 1, 1}
        };
    }


    static bool buttonClick(
        const Evnt& click,
        Evnt& result
    ) {

        for (
            auto it =
                uim::uiLst.rbegin();

            it !=
                uim::uiLst.rend();

            ++it
        ) {

            if (
                it->type !=
                uim::uiType::BUTTON
            )
                continue;


            Vector2 position =
                it->position;


            if (
                it->centered
            ) {

                position.x -=
                    it->size.x *
                    0.5f;

                position.y -=
                    it->size.y *
                    0.5f;
            }


            Rectangle area{

                position.x,
                position.y,

                it->size.x,
                it->size.y
            };


            if (
                !CheckCollisionPointRec(
                    click.mouse,
                    area
                )
            )
                continue;


            result =
                it->event;


            result.port =
                click.port;

            result.valid =
                true;


            return true;
        }


        return false;
    }


    static bool unitClick(
        const Evnt& click,
        Evnt& result
    ) {

        Ray ray =
            GetScreenToWorldRay(
                click.mouse,
                cmr::camera
            );


        con::unit* target =
            nullptr;


        float nearest =
            1000000.0f;


        for (
            auto& unit :
            umg::units
        ) {

            if (
                unit.type !=
                con::Type::DICE
            )
                continue;


            Vector3 half{

                unit.transform.scale.x *
                    0.5f,

                unit.transform.scale.y *
                    0.5f,

                unit.transform.scale.z *
                    0.5f
            };


            BoundingBox box{

                {
                    unit.transform.position.x -
                        half.x,

                    unit.transform.position.y -
                        half.y,

                    unit.transform.position.z -
                        half.z
                },

                {
                    unit.transform.position.x +
                        half.x,

                    unit.transform.position.y +
                        half.y,

                    unit.transform.position.z +
                        half.z
                }
            };


            RayCollision collision =
                GetRayCollisionBox(
                    ray,
                    box
                );


            if (
                !collision.hit
                ||
                collision.distance >=
                    nearest
            )
                continue;


            nearest =
                collision.distance;

            target =
                &unit;
        }


        if (
            target == nullptr
        )
            return false;


        result = {};

        result.id =
            target->id;

        result.type =
            EvntType::KEEP;

        result.port =
            click.port;

        result.local =
            false;

        result.valid =
            true;


        return true;
    }


    static bool resolveClick(
        const Evnt& click,
        Evnt& result
    ) {

        if (
            buttonClick(
                click,
                result
            )
        )
            return true;


        return unitClick(
            click,
            result
        );
    }


    static void queueResolved(
        const Evnt& event
    ) {

        /*
         * Local 결과는 이 기기에서 실행.
         * Host Game Event도 즉시 실행.
         */
        if (
            event.local
            ||
            sttc::host
        ) {

            events.push_back(
                event
            );


            return;
        }


        /*
         * Client Game Event는
         * 다음 Status에서 Host로.
         */
        requests.push_back(
            event
        );
    }


    static void removeGameObjects() {

        std::erase_if(
            umg::units,

            [](
                const con::unit& unit
            ) {

                /*
                 * PLAYER SYSTEM만 유지.
                 */
                return !(
                    unit.type ==
                        con::Type::SYSTEM
                    &&
                    unit.systemType ==
                        con::SystemType::PLAYER
                );
            }
        );
    }


    static void startGame(
        Evnt& event
    ) {

        if (
            !sttc::host
        ) {

            /*
             * Client가 Host로부터
             * 승인된 START를 받은 경우.
             */
            sttc::status =
                sttc::Status::PLAYING;

            sttc::gameStart =
                true;

            return;
        }


        if (
            sttc::status !=
                sttc::Status::ROOM
        ) {

            event.valid =
                false;

            return;
        }


        if (
            event.port !=
                consys::getLocalPort()
        ) {

            event.valid =
                false;

            return;
        }


        if (
            sttc::players.empty()
        ) {

            event.valid =
                false;

            return;
        }


        std::shuffle(
            sttc::players.begin(),
            sttc::players.end(),
            rng
        );


        for (
            size_t i = 0;
            i < sttc::players.size();
            ++i
        ) {

            sttc::players[i].order =
                static_cast<int>(i);
        }


        removeGameObjects();


        /*
         * PLAYER SYSTEM 복구.
         */
        for (
            auto& player :
            sttc::players
        ) {

            player.unitId =
                umg::unitMake(
                    con::Type::SYSTEM,

                    0.0f,

                    defaultTransform(),

                    {0, 0, 0},

                    con::SystemType::PLAYER,

                    player.port
                );
        }


        /*
         * GAME SYSTEM.
         */
        umg::unitMake(
            con::Type::SYSTEM,

            0.0f,

            defaultTransform(),

            {0, 0, 0},

            con::SystemType::GAME
        );


        /*
         * Player별 Camera SYSTEM.
         */
        for (
            const auto& player :
            sttc::players
        ) {

            con::Transform cameraTransform =
                defaultTransform();


            cameraTransform.position =
                {
                    4.0f,
                    3.0f,
                    4.0f
                };


            umg::unitMake(
                con::Type::SYSTEM,

                0.0f,

                cameraTransform,

                {0, 0, 0},

                con::SystemType::CAMERA,

                player.port
            );
        }


        /*
         * TABLE
         */
        con::Transform tableTransform =
            defaultTransform();


        tableTransform.position =
            {
                0.0f,
                -0.5f,
                0.0f
            };


        tableTransform.scale =
            {
                6.0f,
                1.0f,
                6.0f
            };


        umg::unitMake(
            con::Type::TABLE,

            0.0f,

            tableTransform
        );


        /*
         * DICE x5
         */
        for (
            int i = 0;
            i < 5;
            ++i
        ) {

            con::Transform dice =
                defaultTransform();


            dice.position =
                {
                    -2.0f +
                        static_cast<float>(i),

                    0.5f,

                    0.0f
                };


            umg::unitMake(
                con::Type::DICE,

                1.0f,

                dice,

                {0, 0, 0},

                con::SystemType::NONE,

                0,

                1,

                false
            );
        }


        sttc::currentPlayer =
            0;

        sttc::round =
            1;

        sttc::rollCount =
            0;

        sttc::winnerPort =
            0;


        sttc::gameStart =
            true;

        sttc::status =
            sttc::Status::PLAYING;


        cmr::camera.position =
            {4, 3, 4};

        cmr::camera.target =
            {0, 0, 0};

        cmr::camera.up =
            {0, 1, 0};

        cmr::camera.fovy =
            60;

        cmr::camera.projection =
            CAMERA_PERSPECTIVE;
    }


    static bool currentPlayer(
        const Evnt& event
    ) {

        return
            sttc::status ==
                sttc::Status::PLAYING
            &&
            sttc::currentPlayerPort()
                ==
                event.port;
    }


    static void roll(
        Evnt& event
    ) {

        if (
            !sttc::host
        )
            return;


        if (
            !currentPlayer(
                event
            )
        ) {

            event.valid =
                false;

            return;
        }


        if (
            sttc::rollCount >= 3
        ) {

            event.valid =
                false;

            return;
        }


        std::uniform_int_distribution<int>
            faceDistribution(
                1,
                6
            );


        std::uniform_real_distribution<float>
            horizontal(
                -2.5f,
                2.5f
            );


        std::uniform_real_distribution<float>
            vertical(
                4.0f,
                7.0f
            );


        std::uniform_real_distribution<float>
            angle(
                0.0f,
                6.28318530718f
            );


        int index =
            0;


        for (
            auto& unit :
            umg::units
        ) {

            if (
                unit.type !=
                con::Type::DICE
            )
                continue;


            if (
                unit.kept
            ) {

                ++index;

                continue;
            }


            unit.value =
                faceDistribution(
                    rng
                );


            unit.transform.position =
                {
                    -1.5f +
                    0.75f *
                    static_cast<float>(
                        index
                    ),

                    2.5f,

                    horizontal(
                        rng
                    ) * 0.25f
                };


            unit.transform.rotation =
                QuaternionFromEuler(
                    angle(rng),
                    angle(rng),
                    angle(rng)
                );


            unit.velocity =
                {
                    horizontal(rng),
                    vertical(rng),
                    horizontal(rng)
                };


            ++index;
        }


        ++sttc::rollCount;
    }


    static void keep(
        Evnt& event
    ) {

        if (
            !sttc::host
        )
            return;


        if (
            !currentPlayer(
                event
            )
            ||
            sttc::rollCount <= 0
        ) {

            event.valid =
                false;

            return;
        }


        con::unit* dice =
            umg::unitSearch(
                event.id
            );


        if (
            dice == nullptr
            ||
            dice->type !=
                con::Type::DICE
        ) {

            event.valid =
                false;

            return;
        }


        dice->kept =
            !dice->kept;
    }


    static bool scoreType(
        EvntType eventType,
        sttc::ScoreType& result
    ) {

        switch (eventType) {

            case EvntType::SCORE_ONES:
                result =
                    sttc::ScoreType::ONES;
                return true;

            case EvntType::SCORE_TWOS:
                result =
                    sttc::ScoreType::TWOS;
                return true;

            case EvntType::SCORE_THREES:
                result =
                    sttc::ScoreType::THREES;
                return true;

            case EvntType::SCORE_FOURS:
                result =
                    sttc::ScoreType::FOURS;
                return true;

            case EvntType::SCORE_FIVES:
                result =
                    sttc::ScoreType::FIVES;
                return true;

            case EvntType::SCORE_SIXES:
                result =
                    sttc::ScoreType::SIXES;
                return true;

            case EvntType::SCORE_THREE_KIND:
                result =
                    sttc::ScoreType::THREE_KIND;
                return true;

            case EvntType::SCORE_FOUR_KIND:
                result =
                    sttc::ScoreType::FOUR_KIND;
                return true;

            case EvntType::SCORE_FULL_HOUSE:
                result =
                    sttc::ScoreType::FULL_HOUSE;
                return true;

            case EvntType::SCORE_SMALL_STRAIGHT:
                result =
                    sttc::ScoreType::SMALL_STRAIGHT;
                return true;

            case EvntType::SCORE_LARGE_STRAIGHT:
                result =
                    sttc::ScoreType::LARGE_STRAIGHT;
                return true;

            case EvntType::SCORE_YAHTZEE:
                result =
                    sttc::ScoreType::YAHTZEE;
                return true;

            case EvntType::SCORE_CHANCE:
                result =
                    sttc::ScoreType::CHANCE;
                return true;

            default:
                return false;
        }
    }


    static void clearKeep() {

        for (
            auto& unit :
            umg::units
        ) {

            if (
                unit.type ==
                con::Type::DICE
            ) {

                unit.kept =
                    false;
            }
        }
    }


    static void findWinner() {

        int bestScore =
            -1;

        int bestPort =
            0;


        for (
            size_t i = 0;
            i < sttc::players.size();
            ++i
        ) {

            int score =
                sttc::totalScore(
                    static_cast<int>(i)
                );


            if (
                score >
                bestScore
            ) {

                bestScore =
                    score;

                bestPort =
                    sttc::players[i]
                        .port;
            }
        }


        sttc::winnerPort =
            bestPort;
    }


    static void nextTurn() {

        sttc::rollCount =
            0;


        clearKeep();


        ++sttc::currentPlayer;


        if (
            sttc::currentPlayer >=
            static_cast<int>(
                sttc::players.size()
            )
        ) {

            sttc::currentPlayer =
                0;

            ++sttc::round;
        }
    }


    static void recordScore(
        Evnt& event,
        sttc::ScoreType type
    ) {

        if (
            !sttc::host
        )
            return;


        if (
            !currentPlayer(
                event
            )
            ||
            sttc::rollCount <= 0
        ) {

            event.valid =
                false;

            return;
        }


        int playerIndex =
            sttc::currentPlayer;


        int scoreIndex =
            static_cast<int>(
                type
            );


        if (
            sttc::players[
                playerIndex
            ].used[
                scoreIndex
            ]
        ) {

            event.valid =
                false;

            return;
        }


        sttc::players[
            playerIndex
        ].score[
            scoreIndex
        ] =
            sttc::calculateScore(
                type
            );


        sttc::players[
            playerIndex
        ].used[
            scoreIndex
        ] =
            true;


        if (
            sttc::gameFinished()
        ) {

            findWinner();

            sttc::status =
                sttc::Status::ENDING;


            clearKeep();

            sttc::rollCount =
                0;


            return;
        }


        nextTurn();
    }


    ctc::cycLog eventCycle() {

        ctc::cycLog normalLog{
            3,
            0
        };


        for (
            size_t i = 0;
            i < events.size();
            ++i
        ) {

            Evnt event =
                events[i];


            events[i].valid =
                true;


            // =========================
            // SCORE Events
            // =========================

            sttc::ScoreType category{};


            if (
                scoreType(
                    event.type,
                    category
                )
            ) {

                recordScore(
                    events[i],
                    category
                );


                continue;
            }


            switch (
                event.type
            ) {

                case EvntType::CLICK: {

                    if (
                        !event.local
                    ) {

                        events[i].valid =
                            false;

                        break;
                    }


                    Evnt result{};


                    if (
                        !resolveClick(
                            event,
                            result
                        )
                    ) {

                        events[i].valid =
                            false;

                        break;
                    }


                    events[i].valid =
                        false;


                    queueResolved(
                        result
                    );


                    break;
                }


                case EvntType::HOST_ROOM: {

                    if (
                        !event.local
                    ) {

                        events[i].valid =
                            false;

                        break;
                    }


                    consys::closeConnection();

                    sttc::resetSession();


                    consys::fd =
                        consys::ConnectSystem(
                            true
                        );


                    if (
                        consys::fd == -1
                    ) {

                        events[i].valid =
                            false;

                        sttc::status =
                            sttc::Status::MENU;

                        break;
                    }


                    sttc::host =
                        true;

                    sttc::status =
                        sttc::Status::ROOM;


                    sttc::playerAdd(
                        consys::getLocalPort()
                    );


                    break;
                }


                case EvntType::JOIN_ROOM: {

                    if (
                        !event.local
                    ) {

                        events[i].valid =
                            false;

                        break;
                    }


                    consys::closeConnection();

                    sttc::resetSession();


                    consys::fd =
                        consys::ConnectSystem(
                            false
                        );


                    if (
                        consys::fd == -1
                    ) {

                        events[i].valid =
                            false;

                        sttc::status =
                            sttc::Status::MENU;

                        break;
                    }


                    if (
                        !consys::setHostAddress(
                            "192.168.1.126"
                        )
                    ) {

                        consys::closeConnection();

                        events[i].valid =
                            false;

                        sttc::status =
                            sttc::Status::MENU;

                        break;
                    }


                    sttc::host =
                        false;

                    sttc::status =
                        sttc::Status::ROOM;


                    Evnt join{};

                    join.type =
                        EvntType::JOIN;

                    join.local =
                        false;

                    join.valid =
                        true;


                    requests.push_back(
                        join
                    );


                    break;
                }


                case EvntType::BACK_MENU: {

                    if (
                        !event.local
                    ) {

                        events[i].valid =
                            false;

                        break;
                    }


                    consys::closeConnection();

                    sttc::resetSession();


                    sttc::host =
                        false;

                    sttc::status =
                        sttc::Status::MENU;


                    break;
                }


                case EvntType::JOIN: {

                    if (sttc::host) {

                        if (
                            sttc::status !=
                                sttc::Status::ROOM
                        ) {

                            events[i].valid =
                                false;

                            consys::removeClient(
                                event.port
                            );

                            break;
                        }


                        int old =
                            sttc::playerSearch(
                                event.port
                            );


                        if (
                            old == -1
                            &&
                            sttc::players.size() >=
                                sttc::MAX_PLAYERS
                        ) {

                            events[i].valid =
                                false;

                            consys::removeClient(
                                event.port
                            );

                            break;
                        }


                        if (
                            sttc::playerAdd(
                                event.port
                            ) == -1
                        ) {

                            events[i].valid =
                                false;

                            consys::removeClient(
                                event.port
                            );

                            break;
                        }
                    }
                    else {

                        sttc::playerAdd(
                            event.port
                        );
                    }


                    break;
                }


                case EvntType::GAME_START: {

                    startGame(
                        events[i]
                    );


                    break;
                }


                case EvntType::ROLL: {

                    roll(
                        events[i]
                    );


                    break;
                }


                case EvntType::KEEP: {

                    keep(
                        events[i]
                    );


                    break;
                }


                case EvntType::NONE: {

                    events[i].valid =
                        false;


                    break;
                }


                case EvntType::SCORE_ONES:
                case EvntType::SCORE_TWOS:
                case EvntType::SCORE_THREES:
                case EvntType::SCORE_FOURS:
                case EvntType::SCORE_FIVES:
                case EvntType::SCORE_SIXES:
                case EvntType::SCORE_THREE_KIND:
                case EvntType::SCORE_FOUR_KIND:
                case EvntType::SCORE_FULL_HOUSE:
                case EvntType::SCORE_SMALL_STRAIGHT:
                case EvntType::SCORE_LARGE_STRAIGHT:
                case EvntType::SCORE_YAHTZEE:
                case EvntType::SCORE_CHANCE:

                    break;
            }
        }


        if (
            sttc::host
        ) {

            sttc::syncStatusToSystemUnits();
        }


        uim::rebuild();


        /*
         * events는 DistributionCycle에서
         * 사용 후 clear.
         */


        return normalLog;
    }

}