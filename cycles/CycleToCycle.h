//
// Created by octo on 26. 9. 9..
//

#ifndef WIMY_CYCLETOCYCLE_H
#define WIMY_CYCLETOCYCLE_H

#include <string>

namespace ctc {

    struct cycLog {

        int id;

        int result;

        std::string message =
            "s:null_message";
    };


    int CycleToCycle(
        const cycLog& log
    );

}

#endif // WIMY_CYCLETOCYCLE_H