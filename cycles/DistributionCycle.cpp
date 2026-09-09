//
// Created by octo on 26. 9. 9..
//

#include "DistributionCycle.h"

#include "../ConnectSystem.h"
#include "../UnitManager.h"

#include "EventCycle.h"
#include "StatusCycle.h"

namespace dstc {

    ctc::cycLog distributionCycle() {

        ctc::cycLog normalLog{
            5,
            0
        };


        if (
            !sttc::host
        ) {

            evtc::events.clear();

            return normalLog;
        }


        // =========================
        // 승인 Event 배포
        // =========================

        for (
            const auto& event :
            evtc::events
        ) {

            if (
                !event.valid
                ||
                event.local
            )
                continue;


            for (
                const auto& client :
                consys::clients
            ) {

                if (
                    consys::sendEvent(
                        event,
                        client.address
                    ) == -1
                ) {

                    evtc::events.clear();


                    return {
                        5,
                        -1,
                        "s:event_distribution_error"
                    };
                }
            }
        }


        // =========================
        // Unit State 배포
        // =========================

        for (
            const auto& unit :
            umg::units
        ) {

            net::UnitState state{};

            state.id =
                unit.id;

            state.type =
                unit.type;

            state.systemType =
                unit.systemType;

            state.ownerPort =
                unit.ownerPort;

            state.mass =
                unit.mass;

            state.transform =
                unit.transform;

            state.velocity =
                unit.velocity;

            state.value =
                unit.value;

            state.kept =
                unit.kept;

            state.data =
                unit.data;


            for (
                const auto& client :
                consys::clients
            ) {

                if (
                    consys::sendState(
                        state,
                        client.address
                    ) == -1
                ) {

                    evtc::events.clear();


                    return {
                        5,
                        -2,
                        "s:state_distribution_error"
                    };
                }
            }
        }


        evtc::events.clear();


        return normalLog;
    }

}