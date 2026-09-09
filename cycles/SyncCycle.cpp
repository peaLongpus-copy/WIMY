//
// Created by octo on 26. 9. 9..
//

#include "SyncCycle.h"

#include "../UIManager.h"
#include "../UnitManager.h"

#include "StatusCycle.h"

namespace snc {

    std::vector<
        net::UnitState
    > states;


    ctc::cycLog syncCycle() {

        ctc::cycLog normalLog{
            7,
            0
        };


        if (
            sttc::host
        ) {

            states.clear();

            return normalLog;
        }


        for (
            const auto& state :
            states
        ) {

            con::unit* unit =
                umg::unitSearch(
                    state.id
                );


            if (
                unit != nullptr
            ) {

                unit->type =
                    state.type;

                unit->systemType =
                    state.systemType;

                unit->ownerPort =
                    state.ownerPort;

                unit->mass =
                    state.mass;

                unit->transform =
                    state.transform;

                unit->velocity =
                    state.velocity;

                unit->value =
                    state.value;

                unit->kept =
                    state.kept;

                unit->data =
                    state.data;


                continue;
            }


            umg::unitServe(
                state.id,

                state.type,

                state.mass,

                state.transform,

                state.velocity,

                state.systemType,

                state.ownerPort,

                state.value,

                state.kept,

                state.data
            );
        }


        states.clear();


        sttc::syncStatusFromSystemUnits();


        uim::rebuild();


        return normalLog;
    }

}