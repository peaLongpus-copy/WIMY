//
// Created by octo on 26. 9. 9..
//

#include "CycleToCycle.h"

#include <iostream>

namespace ctc {

    int CycleToCycle(
        const cycLog& log
    ) {

        if (
            log.result == 0
        )
            return 0;


        std::cout
            << "CycleToCycle : ID["
            << log.id
            << "] Error ["
            << log.result
            << "] Message : "
            << log.message
            << '\n';


        return -1;
    }

}