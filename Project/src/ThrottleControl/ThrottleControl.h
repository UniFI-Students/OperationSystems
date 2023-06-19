typedef enum ThrottleControlCommandType ThrottleControlCommandType;
typedef struct ThrottleControlCommand ThrottleControlCommand;


enum ThrottleControlCommandType {
    Increment,
};


struct ThrottleControlCommand {
    ThrottleControlCommandType type;
    size_t quantity;
};