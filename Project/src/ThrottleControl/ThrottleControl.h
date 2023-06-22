#ifndef THROTTLE_CONTROL_H
#define THROTTLE_CONTROL_H

typedef enum ThrottleControlCommandType ThrottleControlCommandType;
typedef struct ThrottleControlCommand ThrottleControlCommand;


enum ThrottleControlCommandType {
    Increment,
};


struct ThrottleControlCommand {
    ThrottleControlCommandType type;
    ssize_t quantity;
};

#endif