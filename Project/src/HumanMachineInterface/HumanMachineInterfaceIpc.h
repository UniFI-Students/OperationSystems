#define HUMAN_MACHINE_INTERFACE_SOCKET_NAME "hmi"

typedef enum HmiCommandType HmiCommandType;
typedef struct HumanMachineInterfaceCommand HumanMachineInterfaceCommand;

enum HmiCommandType {
    Start,
    Parking,
    Stop
};

enum HmiExternalCommunicator
{
    CentralEcu
};
struct HumanMachineInterfaceCommand {
    HmiCommandType type;
};
