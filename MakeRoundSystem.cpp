//
// Created by octo on 26. 9. 8..
//

#include "MakeRoundSystem.h"

#include <iostream>

#include "ConnectSystem.h"

#include "cycles/EventCycle.h"
#include "cycles/StatusCycle.h"

namespace mrs {

    int MakeRoundSystem(
        bool hostOnOff,
        const char* hostIp
    ) {

        consys::fd =
            consys::ConnectSystem(
                hostOnOff
            );


        if (consys::fd == -1) {

            std::cout
                << "ConnectSystem Error"
                << std::endl;

            return -1;
        }


        sttc::host =
            hostOnOff;


        /*
         * Client면
         * 앞으로 이벤트를 보낼
         * Host 주소 저장.
         */

        if (!hostOnOff) {

            if (
                hostIp == nullptr
                ||
                !consys::setHostAddress(
                    hostIp
                )
            ) {

                return -2;
            }
        }


        /*
         * Round 시작 이벤트.
         *
         * 아직 PLAYING이 아니므로
         * 로컬 EventCycle로 가도 됨.
         */

        evtc::events.push_back({

            evtc::SYSTEM_ID,

            evtc::EvntType::ROUND_START,

            consys::getLocalPort(),

            true
        });


        return 0;
    }

}