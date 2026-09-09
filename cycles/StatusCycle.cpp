//
// Created by octo on 26. 9. 8..
//

#include "StatusCycle.h"

#include <algorithm>
#include <array>
#include <vector>

#include "../Camera.h"
#include "../ConnectSystem.h"
#include "../Listener.h"
#include "../Packet.h"
#include "../UnitManager.h"

#include "EventCycle.h"
#include "SyncCycle.h"

namespace sttc {

    bool gameStart =
        false;


    bool host =
        false;


    Status status =
        Status::NONE;


    std::vector<Player>
        players;


    int currentPlayer =
        0;


    int round =
        1;


    int rollCount =
        0;


    int winnerPort =
        0;


    struct ReceivedPacket {

        net::Packet packet{};

        sockaddr_in sender{};
    };


    static con::Transform
    systemTransform() {

        con::Transform transform{};


        transform.position =
            {0, 0, 0};


        transform.rotation =
            {0, 0, 0, 1};


        transform.scale =
            {1, 1, 1};


        return transform;
    }


    int playerSearch(
        int port
    ) {

        for (
            size_t i = 0;
            i < players.size();
            ++i
        ) {

            if (
                players[i].port ==
                port
            ) {

                return
                    static_cast<int>(
                        i
                    );
            }
        }


        return -1;
    }


    int playerAdd(
        int port
    ) {

        int found =
            playerSearch(
                port
            );


        if (
            found != -1
        )
            return found;


        if (
            players.size() >=
            MAX_PLAYERS
        )
            return -1;


        Player player{};


        player.port =
            port;


        if (host) {

            player.unitId =
                umg::unitMake(
                    con::Type::SYSTEM,

                    0.0f,

                    systemTransform(),

                    {0, 0, 0},

                    con::SystemType::PLAYER,

                    port
                );
        }


        players.push_back(
            player
        );


        return static_cast<int>(
            players.size() - 1
        );
    }


    int currentPlayerPort() {

        if (
            currentPlayer < 0
            ||
            currentPlayer >=
            static_cast<int>(
                players.size()
            )
        ) {

            return 0;
        }


        return players[
            currentPlayer
        ].port;
    }


    int upperScore(
        int playerIndex
    ) {

        if (
            playerIndex < 0
            ||
            playerIndex >=
            static_cast<int>(
                players.size()
            )
        ) {

            return 0;
        }


        int sum =
            0;


        for (
            int i = 0;
            i < 6;
            ++i
        ) {

            if (
                players[
                    playerIndex
                ].used[i]
            ) {

                sum +=
                    players[
                        playerIndex
                    ].score[i];
            }
        }


        return sum;
    }


    int totalScore(
        int playerIndex
    ) {

        if (
            playerIndex < 0
            ||
            playerIndex >=
            static_cast<int>(
                players.size()
            )
        ) {

            return 0;
        }


        int sum =
            0;


        for (
            int i = 0;
            i < SCORE_COUNT;
            ++i
        ) {

            if (
                players[
                    playerIndex
                ].used[i]
            ) {

                sum +=
                    players[
                        playerIndex
                    ].score[i];
            }
        }


        if (
            upperScore(
                playerIndex
            ) >= 63
        ) {

            sum +=
                35;
        }


        return sum;
    }


    bool gameFinished() {

        if (
            players.empty()
        )
            return false;


        for (
            const auto& player :
            players
        ) {

            for (
                bool used :
                player.used
            ) {

                if (
                    !used
                )
                    return false;
            }
        }


        return true;
    }


    int calculateScore(
        ScoreType type
    ) {

        std::array<int, 7>
            count{};


        int sum =
            0;


        int diceCount =
            0;


        for (
            const auto& unit :
            umg::units
        ) {

            if (
                unit.type !=
                con::Type::DICE
            )
                continue;


            if (
                unit.value < 1
                ||
                unit.value > 6
            )
                continue;


            ++count[
                unit.value
            ];


            sum +=
                unit.value;


            ++diceCount;
        }


        if (
            diceCount != 5
        )
            return 0;


        switch (type) {

            case ScoreType::ONES:
            case ScoreType::TWOS:
            case ScoreType::THREES:
            case ScoreType::FOURS:
            case ScoreType::FIVES:
            case ScoreType::SIXES: {

                int face =
                    static_cast<int>(
                        type
                    ) + 1;


                return
                    face *
                    count[face];
            }


            case ScoreType::THREE_KIND: {

                for (
                    int face = 1;
                    face <= 6;
                    ++face
                ) {

                    if (
                        count[face] >= 3
                    )
                        return sum;
                }


                return 0;
            }


            case ScoreType::FOUR_KIND: {

                for (
                    int face = 1;
                    face <= 6;
                    ++face
                ) {

                    if (
                        count[face] >= 4
                    )
                        return sum;
                }


                return 0;
            }


            case ScoreType::FULL_HOUSE: {

                bool two =
                    false;


                bool three =
                    false;


                for (
                    int face = 1;
                    face <= 6;
                    ++face
                ) {

                    if (
                        count[face] == 2
                    )
                        two = true;


                    if (
                        count[face] == 3
                    )
                        three = true;
                }


                return
                    two &&
                    three
                    ? 25
                    : 0;
            }


            case ScoreType::SMALL_STRAIGHT: {

                bool a =
                    count[1]
                    &&
                    count[2]
                    &&
                    count[3]
                    &&
                    count[4];


                bool b =
                    count[2]
                    &&
                    count[3]
                    &&
                    count[4]
                    &&
                    count[5];


                bool c =
                    count[3]
                    &&
                    count[4]
                    &&
                    count[5]
                    &&
                    count[6];


                return
                    a ||
                    b ||
                    c
                    ? 30
                    : 0;
            }


            case ScoreType::LARGE_STRAIGHT: {

                bool a =
                    count[1]
                    &&
                    count[2]
                    &&
                    count[3]
                    &&
                    count[4]
                    &&
                    count[5];


                bool b =
                    count[2]
                    &&
                    count[3]
                    &&
                    count[4]
                    &&
                    count[5]
                    &&
                    count[6];


                return
                    a ||
                    b
                    ? 40
                    : 0;
            }


            case ScoreType::YAHTZEE: {

                for (
                    int face = 1;
                    face <= 6;
                    ++face
                ) {

                    if (
                        count[face] == 5
                    )
                        return 50;
                }


                return 0;
            }


            case ScoreType::CHANCE:

                return sum;


            case ScoreType::COUNT:

                return 0;
        }


        return 0;
    }


    const char* scoreName(
        ScoreType type
    ) {

        switch (type) {

            case ScoreType::ONES:
                return "Ones";

            case ScoreType::TWOS:
                return "Twos";

            case ScoreType::THREES:
                return "Threes";

            case ScoreType::FOURS:
                return "Fours";

            case ScoreType::FIVES:
                return "Fives";

            case ScoreType::SIXES:
                return "Sixes";

            case ScoreType::THREE_KIND:
                return "Three Kind";

            case ScoreType::FOUR_KIND:
                return "Four Kind";

            case ScoreType::FULL_HOUSE:
                return "Full House";

            case ScoreType::SMALL_STRAIGHT:
                return "Small Straight";

            case ScoreType::LARGE_STRAIGHT:
                return "Large Straight";

            case ScoreType::YAHTZEE:
                return "Yahtzee";

            case ScoreType::CHANCE:
                return "Chance";

            case ScoreType::COUNT:
                return "";
        }


        return "";
    }


    void resetSession() {

        players.clear();


        currentPlayer =
            0;


        round =
            1;


        rollCount =
            0;


        winnerPort =
            0;


        gameStart =
            false;


        umg::unitClear();
    }


    void syncStatusToSystemUnits() {

        if (
            !host
        )
            return;


        for (
            size_t i = 0;
            i < players.size();
            ++i
        ) {

            Player& player =
                players[i];


            con::unit* unit =
                umg::unitSearch(
                    player.unitId
                );


            if (
                unit == nullptr
            ) {

                unit =
                    umg::systemSearch(
                        con::SystemType::PLAYER,

                        player.port
                    );
            }


            if (
                unit == nullptr
            ) {

                player.unitId =
                    umg::unitMake(
                        con::Type::SYSTEM,

                        0.0f,

                        systemTransform(),

                        {0, 0, 0},

                        con::SystemType::PLAYER,

                        player.port
                    );


                unit =
                    umg::unitSearch(
                        player.unitId
                    );
            }


            if (
                unit == nullptr
            )
                continue;


            player.unitId =
                unit->id;


            int usedMask =
                0;


            for (
                int scoreIndex = 0;
                scoreIndex < SCORE_COUNT;
                ++scoreIndex
            ) {

                unit->data[
                    scoreIndex
                ] =
                    player.score[
                        scoreIndex
                    ];


                if (
                    player.used[
                        scoreIndex
                    ]
                ) {

                    usedMask |=
                        1 << scoreIndex;
                }
            }


            unit->data[13] =
                usedMask;


            unit->data[14] =
                player.order;


            unit->data[15] =
                totalScore(
                    static_cast<int>(
                        i
                    )
                );
        }


        con::unit* game =
            umg::systemSearch(
                con::SystemType::GAME
            );


        if (
            game != nullptr
        ) {

            game->data[0] =
                currentPlayer;


            game->data[1] =
                round;


            game->data[2] =
                rollCount;


            game->data[3] =
                static_cast<int>(
                    status
                );


            game->data[4] =
                winnerPort;


            game->data[5] =
                static_cast<int>(
                    players.size()
                );
        }
    }


    void syncStatusFromSystemUnits() {

        if (
            host
        )
            return;


        std::vector<Player>
            syncedPlayers;


        for (
            const auto& unit :
            umg::units
        ) {

            if (
                unit.type !=
                    con::Type::SYSTEM
                ||
                unit.systemType !=
                    con::SystemType::PLAYER
            ) {

                continue;
            }


            Player player{};


            player.port =
                unit.ownerPort;


            player.unitId =
                unit.id;


            player.order =
                unit.data[14];


            int usedMask =
                unit.data[13];


            for (
                int i = 0;
                i < SCORE_COUNT;
                ++i
            ) {

                player.score[i] =
                    unit.data[i];


                player.used[i] =
                    (
                        usedMask
                        &
                        (
                            1 << i
                        )
                    ) != 0;
            }


            syncedPlayers.push_back(
                player
            );
        }


        if (
            !syncedPlayers.empty()
        ) {

            std::sort(
                syncedPlayers.begin(),
                syncedPlayers.end(),

                [](
                    const Player& a,
                    const Player& b
                ) {

                    if (
                        a.order >= 0
                        &&
                        b.order >= 0
                    ) {

                        return
                            a.order <
                            b.order;
                    }


                    if (
                        a.order >= 0
                    )
                        return true;


                    if (
                        b.order >= 0
                    )
                        return false;


                    return
                        a.unitId <
                        b.unitId;
                }
            );


            players =
                syncedPlayers;
        }


        con::unit* game =
            umg::systemSearch(
                con::SystemType::GAME
            );


        if (
            game != nullptr
        ) {

            currentPlayer =
                game->data[0];


            round =
                game->data[1];


            rollCount =
                game->data[2];


            status =
                static_cast<Status>(
                    game->data[3]
                );


            winnerPort =
                game->data[4];


            gameStart =
                status ==
                    Status::PLAYING
                ||
                status ==
                    Status::ENDING;
        }


        int localPort =
            consys::getLocalPort();


        con::unit* camera =
            umg::systemSearch(
                con::SystemType::CAMERA,

                localPort
            );


        if (
            camera != nullptr
        ) {

            cmr::camera.position =
                camera
                    ->transform
                    .position;


            cmr::camera.target =
                {0, 0, 0};


            cmr::camera.up =
                {0, 1, 0};


            cmr::camera.fovy =
                60.0f;


            cmr::camera.projection =
                CAMERA_PERSPECTIVE;
        }
    }


    ctc::cycLog statusCycle() {

        ctc::cycLog normalLog{
            1,
            0
        };


        std::vector<
            ReceivedPacket
        > packetBuffer;


        // ====================================
        // Packet 수신
        // ====================================

        if (
            consys::isOpen()
        ) {

            while (true) {

                ReceivedPacket
                    received{};


                int result =
                    consys::receivePacket(
                        received.packet,

                        received.sender
                    );


                // 현재 Queue 비었음
                if (
                    result == 0
                )
                    break;


                // Socket Error
                if (
                    result == -1
                ) {

                    return {
                        1,
                        -1,
                        "s:receive_packet_error"
                    };
                }


                // Packet 크기 이상
                if (
                    result == -2
                ) {

                    continue;
                }


                /*
                 * Client는
                 * 지정 Host Packet만 신뢰.
                 */
                if (
                    !host
                    &&
                    !consys::isHostAddress(
                        received.sender
                    )
                ) {

                    continue;
                }


                packetBuffer.push_back(
                    received
                );
            }
        }


        // ====================================
        // Packet 분류
        // ====================================

        for (
            auto& received :
            packetBuffer
        ) {

            switch (
                received.packet.type
            ) {

                case net::DataType::EVENT: {

                    evtc::Evnt event =
                        received
                            .packet
                            .event;


                    /*
                     * Network Event는
                     * Local일 수 없음.
                     */
                    event.local =
                        false;


                    /*
                     * Host가 Client Event를
                     * 받을 때 Packet port는
                     * 신뢰하지 않음.
                     */
                    if (host) {

                        event.port =
                            ntohs(
                                received
                                    .sender
                                    .sin_port
                            );


                        /*
                         * JOIN 요청에서만
                         * Client 등록.
                         */
                        if (
                            event.type ==
                            evtc::EvntType::JOIN
                        ) {

                            consys::registerClient(
                                received.sender
                            );
                        }
                    }


                    evtc::events.push_back(
                        event
                    );


                    break;
                }


                case net::DataType::STATE: {

                    if (
                        !host
                    ) {

                        snc::states.push_back(
                            received
                                .packet
                                .state
                        );
                    }


                    break;
                }
            }
        }


        // ====================================
        // EventCycle -> Host
        // ====================================

        if (
            !host
            &&
            consys::isOpen()
        ) {

            for (
                const auto& event :
                evtc::requests
            ) {

                if (
                    consys::sendEventToHost(
                        event
                    ) == -1
                ) {

                    evtc::requests.clear();


                    return {
                        1,
                        -2,
                        "s:event_request_send_error"
                    };
                }
            }
        }


        evtc::requests.clear();


        // ====================================
        // Listener 분류
        // ====================================

        for (
            auto& listener :
            lst::listeners
        ) {

            /*
             * Mouse / Keyboard 등
             * Raw local Event.
             */
            if (
                listener.local
            ) {

                evtc::events.push_back(
                    listener
                );


                continue;
            }


            /*
             * Client의 Game Event.
             */
            if (
                !host
                &&
                consys::isOpen()
            ) {

                if (
                    consys::sendEventToHost(
                        listener
                    ) == -1
                ) {

                    lst::listeners.clear();


                    return {
                        1,
                        -3,
                        "s:event_send_error"
                    };
                }


                continue;
            }


            /*
             * Host의 Game Event.
             */
            evtc::events.push_back(
                listener
            );
        }


        lst::listeners.clear();


        return normalLog;
    }

}