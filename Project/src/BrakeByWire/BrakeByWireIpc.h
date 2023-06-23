#ifndef OS_BREAK_BY_WIRE_H
#define OS_BREAK_BY_WIRE_H

#define BREAK_BY_WIRE_UNIX_SOCKET_NAME "bbwSocketFd"
#define BRAKE_BY_WIRE_INET_SOCKET_PORT 1022


enum BreakByWireRequester {
    CentralEcuToBbwRequester
} typedef BreakByWireRequester;


enum BrakeByWireCommandType {
    Brake,
} typedef BrakeByWireCommandType;


struct BrakeByWireCommand {
    BrakeByWireCommandType type;
    ssize_t quantity;
} typedef BrakeByWireCommand;

#endif