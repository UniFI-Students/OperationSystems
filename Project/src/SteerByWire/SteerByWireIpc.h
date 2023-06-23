#ifndef STEER_BY_WIRE_H
#define STEER_BY_WIRE_H

#define STEER_BY_WIRE_UNIX_SOCKET_NAME "sbwSocket"
#define STEER_BY_WIRE_INET_SOCKET_PORT 1023


enum SteerByWireRequester{
    CentralEcuToSbwRequester,
} typedef SteerByWireRequester;

enum SteerByWireCommandType {
    Left,
    Right
} typedef SteerByWireCommandType;

struct SteerByWireCommand {
    SteerByWireCommandType type;
} typedef SteerByWireCommand;

#endif