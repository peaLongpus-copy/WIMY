//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_LISTENER_H
#define WIMY_LISTENER_H

#include <vector>

#include "cycles/CycleToCycle.h"
#include "cycles/EventCycle.h"

namespace lst {

    extern std::vector<
        evtc::Evnt
    > listeners;


    ctc::cycLog listenerCycle();

}

#endif // WIMY_LISTENER_H