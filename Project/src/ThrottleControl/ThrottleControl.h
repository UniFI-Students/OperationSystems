typedef enum ThrottleControlCommandType ThrottleControlCommandType;
typedef struct ThrottleControlCommand ThrottleControlCommand;


enum ThrottleControlCommandType {
    Increment,
};


struct ThrottleControlCommand {
    ThrottleControlCommandType type;
    ssize_t quantity;
};