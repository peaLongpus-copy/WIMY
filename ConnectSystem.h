//
// Created by octo on 26. 9. 8..
//

#ifndef WIMY_CONNECTSYSTEM_H
#define WIMY_CONNECTSYSTEM_H

#include <cstdint>
#include <vector>


// ====================================
// Windows Socket
// ====================================

#ifdef _WIN32

/*
 * winsock2.h가 windows.h를 불러오면서
 *
 * Rectangle
 * DrawText
 * CloseWindow
 * ShowCursor
 * PlaySound
 *
 * 등의 Windows API 이름과
 * raylib 이름이 충돌하는 것을 방지.
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef NOGDI
#define NOGDI
#endif

#ifndef NOUSER
#define NOUSER
#endif


#include <winsock2.h>
#include <ws2tcpip.h>


#else

// ====================================
// Linux Socket
// ====================================

#include <netinet/in.h>

#endif


#include "Packet.h"


namespace consys {

    constexpr int HOST_PORT =
        7777;


    /*
     * Linux:
     * int
     *
     * Windows:
     * SOCKET
     *
     * 둘 다 저장 가능한
     * pointer-size 정수.
     */
    using SocketHandle =
        std::intptr_t;


    constexpr SocketHandle
        INVALID_SOCKET_HANDLE =
            -1;


    struct Client {

        sockaddr_in address{};
    };


    // UDP socket
    extern SocketHandle fd;


    // Client가 접속할 Host
    extern sockaddr_in hostAddress;


    // Host가 관리하는 Client
    extern std::vector<Client>
        clients;


    // UDP socket 생성
    SocketHandle ConnectSystem(
        bool hosted
    );


    // socket 사용 여부
    bool isOpen();


    // Host IP 설정
    bool setHostAddress(
        const char* ip
    );


    // 현재 local UDP port
    int getLocalPort();


    // 지정 주소로 Event 전송
    int sendEvent(
        const evtc::Evnt& event,
        const sockaddr_in& target
    );


    // Host로 Event 전송
    int sendEventToHost(
        const evtc::Evnt& event
    );


    // 지정 주소로 State 전송
    int sendState(
        const net::UnitState& state,
        const sockaddr_in& target
    );


    /*
     * Packet 하나 수신
     *
     *  1 = 정상
     *  0 = 현재 받을 Packet 없음
     * -1 = socket 오류
     * -2 = Packet 크기 오류
     */
    int receivePacket(
        net::Packet& packet,
        sockaddr_in& sender
    );


    // Client 주소 등록
    void registerClient(
        const sockaddr_in& address
    );


    // Client 제거
    void removeClient(
        int port
    );


    // 해당 주소가 Host인지 확인
    bool isHostAddress(
        const sockaddr_in& address
    );


    // Socket 종료
    void closeConnection();

}

#endif // WIMY_CONNECTSYSTEM_H