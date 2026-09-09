#include "BackSystem.h"
#include "Camera.h"
#include "Listener.h"

#include "cycles/CycleToCycle.h"
#include "cycles/EventCycle.h"
#include "cycles/SyncCycle.h"
#include "cycles/PhysicsCycle.h"
#include "cycles/DistributionCycle.h"
#include "cycles/RenderCycle.h"
#include "cycles/UIRenderCycle.h"
#include "cycles/StatusCycle.h"


int main() {

    return bst::gameStart([] {


        // 1. 로컬 입력 수집
        lst::listenerCycle();


        // 2. Network 패킷 수집 / 분류
        ctc::CycleToCycle(
            sttc::statusCycle()
        );


        // 3. Event 실행 및 valid 확정
        ctc::CycleToCycle(
            evtc::eventCycle()
        );


        // 4. Client만 Host 상태 적용
        ctc::CycleToCycle(
            snc::syncCycle()
        );


        // 5. Host만 authoritative physics
        ctc::CycleToCycle(
            phyc::physicsCycle()
        );


        // 6. Host가 Event + 전체 Unit 상태 배포
        ctc::CycleToCycle(
            dstc::distributionCycle()
        );


        // 7. 화면 출력
        BeginDrawing();

        ClearBackground(BLACK);


        ctc::CycleToCycle(
            rdc::renderCycle(
                cmr::camera
            )
        );


        ctc::CycleToCycle(
            uirc::uiRenderCycle()
        );


        EndDrawing();


    }, true);
}