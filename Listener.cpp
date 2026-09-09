//
// Created by octo on 26. 9. 9..
//

#include "Listener.h"

#include "raylib.h"

#include "ConnectSystem.h"

namespace lst {

    std::vector<
        evtc::Evnt
    > listeners;


    ctc::cycLog listenerCycle() {

        ctc::cycLog normalLog{
            0,
            0
        };


        if (
            IsMouseButtonPressed(
                MOUSE_BUTTON_LEFT
            )
        ) {

            evtc::Evnt event{};

            event.id =
                evtc::SYSTEM_ID;

            event.type =
                evtc::EvntType::CLICK;

            event.port =
                consys::getLocalPort();

            event.local =
                true;

            event.mouse =
                GetMousePosition();

            event.valid =
                true;


            listeners.push_back(
                event
            );
        }


        return normalLog;
    }

}