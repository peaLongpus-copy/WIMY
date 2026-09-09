//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_SYNCCYCLE_H
#define WIMY_SYNCCYCLE_H

#include <vector>

#include "../Packet.h"
#include "CycleToCycle.h"

namespace snc {

    extern std::vector<
        net::UnitState
    > states;


    ctc::cycLog syncCycle();

}

#endif // WIMY_SYNCCYCLE_H