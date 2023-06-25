#ifndef STEER_BY_WIRE_IPC_H
#define STEER_BY_WIRE_IPC_H

enum SteerByWireRequester {
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