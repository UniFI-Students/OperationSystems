typedef enum BrakeByWireCommandType BrakeByWireCommandType;
typedef struct BrakeByWireCommand BrakeByWireCommand;


enum BrakeByWireCommandType {
    Brake,
};


struct BrakeByWireCommand {
    BrakeByWireCommandType type;
    ssize_t quantity;
};