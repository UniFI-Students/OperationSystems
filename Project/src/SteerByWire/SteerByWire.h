#ifndef STEER_BY_WIRE_H
#define STEER_BY_WIRE_H

typedef enum SteerByWireCommandType SteerByWireCommandType;
typedef struct SteerByWireCommand SteerByWireCommand;

enum SteerByWireCommandType {
    Left,
    Right,
    NoAction
};

struct SteerByWireCommand {
    SteerByWireCommandType type;
};

#endif