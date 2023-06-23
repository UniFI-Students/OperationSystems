#ifndef OS_THROTTLE_CONTROL_H
#define OS_THROTTLE_CONTROL_H


enum ThrottleControlRequester {
    CentralEcuToTcRequester,
} typedef ThrottleControlRequester;

enum ThrottleControlCommandType {
    Increment,
} typedef ThrottleControlCommandType;


struct ThrottleControlCommand {
    ThrottleControlCommandType type;
    ssize_t quantity;
} typedef ThrottleControlCommand;

#endif