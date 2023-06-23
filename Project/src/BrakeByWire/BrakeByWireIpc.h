#ifndef OS_BREAK_BY_WIRE_H
#define OS_BREAK_BY_WIRE_H




enum BreakByWireRequester {
    CentralEcuToBbwRequester
} typedef BreakByWireRequester;


enum BrakeByWireCommandType {
    Decrement,
} typedef BrakeByWireCommandType;


struct BrakeByWireCommand {
    BrakeByWireCommandType type;
    ssize_t quantity;
} typedef BrakeByWireCommand;

#endif