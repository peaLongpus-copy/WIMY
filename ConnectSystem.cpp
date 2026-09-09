//
// Created by octo on 26. 9. 8..
//

#include "ConnectSystem.h"

#include <algorithm>
#include <cstring>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#endif


namespace consys {

    SocketHandle fd =
        INVALID_SOCKET_HANDLE;


    sockaddr_in hostAddress{};


    std::vector<Client>
        clients;


#ifdef _WIN32

    static bool wsaReady =
        false;

#endif


    // ====================================
    // Platform socket 초기화
    // ====================================

    static bool socketSystemInit() {

#ifdef _WIN32

        if (
            wsaReady
        )
            return true;


        WSADATA wsaData{};


        int result =
            WSAStartup(
                MAKEWORD(
                    2,
                    2
                ),

                &wsaData
            );


        if (
            result != 0
        )
            return false;


        wsaReady =
            true;

#endif


        return true;
    }


    // ====================================
    // Platform socket system 종료
    // ====================================

    static void socketSystemClose() {

#ifdef _WIN32

        if (
            !wsaReady
        )
            return;


        WSACleanup();


        wsaReady =
            false;

#endif

    }


    // ====================================
    // Native socket 변환
    // ====================================

#ifdef _WIN32

    static SOCKET nativeSocket() {

        return static_cast<SOCKET>(
            fd
        );
    }

#else

    static int nativeSocket() {

        return static_cast<int>(
            fd
        );
    }

#endif


    // ====================================
    // Socket close
    // ====================================

    static void closeSocket(
        SocketHandle socketHandle
    ) {

        if (
            socketHandle ==
            INVALID_SOCKET_HANDLE
        )
            return;


#ifdef _WIN32

        closesocket(
            static_cast<SOCKET>(
                socketHandle
            )
        );

#else

        close(
            static_cast<int>(
                socketHandle
            )
        );

#endif
    }


    // ====================================
    // Non-blocking 설정
    // ====================================

    static bool setNonBlocking(
        SocketHandle socketHandle
    ) {

#ifdef _WIN32

        u_long mode =
            1;


        return ioctlsocket(
            static_cast<SOCKET>(
                socketHandle
            ),

            FIONBIO,

            &mode
        ) == 0;

#else

        int native =
            static_cast<int>(
                socketHandle
            );


        int flags =
            fcntl(
                native,
                F_GETFL,
                0
            );


        if (
            flags == -1
        )
            return false;


        return fcntl(
            native,
            F_SETFL,
            flags | O_NONBLOCK
        ) != -1;

#endif
    }


    // ====================================
    // WouldBlock 확인
    // ====================================

    static bool wouldBlock() {

#ifdef _WIN32

        return
            WSAGetLastError()
            ==
            WSAEWOULDBLOCK;

#else

        return
            errno == EAGAIN
            ||
            errno == EWOULDBLOCK;

#endif
    }


    // ====================================
    // UDP Socket 생성
    // ====================================

    SocketHandle ConnectSystem(
        bool hosted
    ) {

        if (
            !socketSystemInit()
        ) {

            return
                INVALID_SOCKET_HANDLE;
        }


#ifdef _WIN32

        SOCKET socketFd =
            socket(
                AF_INET,
                SOCK_DGRAM,
                IPPROTO_UDP
            );


        if (
            socketFd ==
            INVALID_SOCKET
        ) {

            socketSystemClose();

            return
                INVALID_SOCKET_HANDLE;
        }


        SocketHandle resultHandle =
            static_cast<SocketHandle>(
                socketFd
            );

#else

        int socketFd =
            socket(
                AF_INET,
                SOCK_DGRAM,
                0
            );


        if (
            socketFd == -1
        ) {

            socketSystemClose();

            return
                INVALID_SOCKET_HANDLE;
        }


        SocketHandle resultHandle =
            static_cast<SocketHandle>(
                socketFd
            );

#endif


        // 주소 재사용
        int reuse =
            1;


#ifdef _WIN32

        setsockopt(
            socketFd,

            SOL_SOCKET,
            SO_REUSEADDR,

            reinterpret_cast<
                const char*
            >(
                &reuse
            ),

            sizeof(reuse)
        );

#else

        setsockopt(
            socketFd,

            SOL_SOCKET,
            SO_REUSEADDR,

            &reuse,

            sizeof(reuse)
        );

#endif


        if (
            !setNonBlocking(
                resultHandle
            )
        ) {

            closeSocket(
                resultHandle
            );


            socketSystemClose();


            return
                INVALID_SOCKET_HANDLE;
        }


        // ====================================
        // Local 주소 Bind
        // ====================================

        sockaddr_in localAddress{};


        localAddress.sin_family =
            AF_INET;


        /*
         * Host:
         * 고정 7777
         *
         * Client:
         * OS가 빈 Port 자동 할당.
         */
        localAddress.sin_port =
            htons(
                hosted
                    ? HOST_PORT
                    : 0
            );


        localAddress
            .sin_addr
            .s_addr =
                htonl(
                    INADDR_ANY
                );


#ifdef _WIN32

        int bindResult =
            bind(
                socketFd,

                reinterpret_cast<
                    const sockaddr*
                >(
                    &localAddress
                ),

                sizeof(
                    localAddress
                )
            );

#else

        int bindResult =
            bind(
                socketFd,

                reinterpret_cast<
                    sockaddr*
                >(
                    &localAddress
                ),

                sizeof(
                    localAddress
                )
            );

#endif


        if (
            bindResult != 0
        ) {

            closeSocket(
                resultHandle
            );


            socketSystemClose();


            return
                INVALID_SOCKET_HANDLE;
        }


        return
            resultHandle;
    }


    // ====================================
    // Socket 상태
    // ====================================

    bool isOpen() {

        return
            fd !=
            INVALID_SOCKET_HANDLE;
    }


    // ====================================
    // Host IP
    // ====================================

    bool setHostAddress(
        const char* ip
    ) {

        hostAddress = {};


        hostAddress.sin_family =
            AF_INET;


        hostAddress.sin_port =
            htons(
                HOST_PORT
            );


        int result =
            inet_pton(
                AF_INET,

                ip,

                &hostAddress
                    .sin_addr
            );


        return
            result == 1;
    }


    // ====================================
    // Local Port
    // ====================================

    int getLocalPort() {

        if (
            !isOpen()
        )
            return 0;


        sockaddr_in address{};


#ifdef _WIN32

        int addressLength =
            sizeof(address);


        int result =
            getsockname(
                nativeSocket(),

                reinterpret_cast<
                    sockaddr*
                >(
                    &address
                ),

                &addressLength
            );

#else

        socklen_t addressLength =
            sizeof(address);


        int result =
            getsockname(
                nativeSocket(),

                reinterpret_cast<
                    sockaddr*
                >(
                    &address
                ),

                &addressLength
            );

#endif


        if (
            result != 0
        )
            return 0;


        return ntohs(
            address.sin_port
        );
    }


    // ====================================
    // Packet 전송
    // ====================================

    static int sendPacket(
        const net::Packet& packet,
        const sockaddr_in& target
    ) {

        if (
            !isOpen()
        )
            return -1;


#ifdef _WIN32

        int result =
            sendto(
                nativeSocket(),

                reinterpret_cast<
                    const char*
                >(
                    &packet
                ),

                static_cast<int>(
                    sizeof(packet)
                ),

                0,

                reinterpret_cast<
                    const sockaddr*
                >(
                    &target
                ),

                sizeof(target)
            );


        if (
            result ==
            SOCKET_ERROR
        ) {

            return -1;
        }

#else

        ssize_t result =
            sendto(
                nativeSocket(),

                &packet,

                sizeof(packet),

                0,

                reinterpret_cast<
                    const sockaddr*
                >(
                    &target
                ),

                sizeof(target)
            );


        if (
            result == -1
        )
            return -1;

#endif


        if (
            result !=
            static_cast<
                decltype(result)
            >(
                sizeof(packet)
            )
        ) {

            return -1;
        }


        return 0;
    }


    // ====================================
    // Event 전송
    // ====================================

    int sendEvent(
        const evtc::Evnt& event,
        const sockaddr_in& target
    ) {

        net::Packet packet{};


        packet.type =
            net::DataType::EVENT;


        packet.event =
            event;


        return sendPacket(
            packet,
            target
        );
    }


    // ====================================
    // Host Event 전송
    // ====================================

    int sendEventToHost(
        const evtc::Evnt& event
    ) {

        return sendEvent(
            event,
            hostAddress
        );
    }


    // ====================================
    // State 전송
    // ====================================

    int sendState(
        const net::UnitState& state,
        const sockaddr_in& target
    ) {

        net::Packet packet{};


        packet.type =
            net::DataType::STATE;


        packet.state =
            state;


        return sendPacket(
            packet,
            target
        );
    }


    // ====================================
    // Packet 수신
    // ====================================

    int receivePacket(
        net::Packet& packet,
        sockaddr_in& sender
    ) {

        if (
            !isOpen()
        )
            return 0;


        sender = {};


#ifdef _WIN32

        int senderLength =
            sizeof(sender);


        int result =
            recvfrom(
                nativeSocket(),

                reinterpret_cast<
                    char*
                >(
                    &packet
                ),

                static_cast<int>(
                    sizeof(packet)
                ),

                0,

                reinterpret_cast<
                    sockaddr*
                >(
                    &sender
                ),

                &senderLength
            );


        if (
            result ==
            SOCKET_ERROR
        ) {

            if (
                wouldBlock()
            )
                return 0;


            return -1;
        }

#else

        socklen_t senderLength =
            sizeof(sender);


        ssize_t result =
            recvfrom(
                nativeSocket(),

                &packet,

                sizeof(packet),

                0,

                reinterpret_cast<
                    sockaddr*
                >(
                    &sender
                ),

                &senderLength
            );


        if (
            result == -1
        ) {

            if (
                wouldBlock()
            )
                return 0;


            return -1;
        }

#endif


        if (
            result !=
            static_cast<
                decltype(result)
            >(
                sizeof(
                    net::Packet
                )
            )
        ) {

            return -2;
        }


        return 1;
    }


    // ====================================
    // Client 등록
    // ====================================

    void registerClient(
        const sockaddr_in& address
    ) {

        for (
            const auto& client :
            clients
        ) {

            if (
                client.address
                    .sin_addr
                    .s_addr
                ==
                address
                    .sin_addr
                    .s_addr
                &&
                client.address
                    .sin_port
                ==
                address
                    .sin_port
            ) {

                return;
            }
        }


        clients.push_back({
            address
        });
    }


    // ====================================
    // Client 삭제
    // ====================================

    void removeClient(
        int port
    ) {

        std::erase_if(
            clients,

            [port](
                const Client& client
            ) {

                return ntohs(
                    client
                        .address
                        .sin_port
                ) == port;
            }
        );
    }


    // ====================================
    // Host 주소 확인
    // ====================================

    bool isHostAddress(
        const sockaddr_in& address
    ) {

        return
            address.sin_family ==
                hostAddress.sin_family
            &&
            address.sin_port ==
                hostAddress.sin_port
            &&
            address
                .sin_addr
                .s_addr
            ==
            hostAddress
                .sin_addr
                .s_addr;
    }


    // ====================================
    // Connection 종료
    // ====================================

    void closeConnection() {

        if (
            isOpen()
        ) {

            closeSocket(
                fd
            );


            fd =
                INVALID_SOCKET_HANDLE;
        }


        clients.clear();


        hostAddress = {};


        socketSystemClose();
    }

}