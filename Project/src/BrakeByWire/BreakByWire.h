#ifndef OS_BREAK_BY_WIRE_H
#define OS_BREAK_BY_WIRE_H

typedef enum BrakeByWireCommandType BrakeByWireCommandType;
typedef struct BrakeByWireCommand BrakeByWireCommand;


enum BrakeByWireCommandType {
    Brake,
};


struct BrakeByWireCommand {
    BrakeByWireCommandType type;
    ssize_t quantity;
};

#endif