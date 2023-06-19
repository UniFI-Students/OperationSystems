typedef enum HumanMachineInterfaceCommandType HumanMachineInterfaceCommandType;
typedef struct HumanMachineInterfaceCommand HumanMachineInterfaceCommand;

enum HumanMachineInterfaceCommandType {
    Start,
    Parking,
    Stop
};
struct HumanMachineInterfaceCommand {
    HumanMachineInterfaceCommandType type;
};
