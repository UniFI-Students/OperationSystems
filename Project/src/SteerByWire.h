
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