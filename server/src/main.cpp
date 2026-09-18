#include "cricket/match.hpp"
#include "cricket/server/room.hpp"

#include <iostream>

int main() {
    std::cout << "Boundary Cricket authoritative server shell\n"
              << "Core version: " << cricket::core_version << '\n'
              << "Network listener: not enabled in the architecture milestone\n"
              << "Next milestone: HTTP/WebSocket room service on port 8080\n";

    cricket::server::Room demonstration_room{"ABCDEFGH"};
    demonstration_room.join({"Host", "local-development-session"});

    std::cout << "Room model initialized with code " << demonstration_room.code() << '\n';
    return 0;
}