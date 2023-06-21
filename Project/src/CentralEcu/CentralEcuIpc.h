#define CENTRAL_ECU_UNIX_SOCKET_NAME "cEcuSocket"
#define CENTRAL_ECU_INET_SOCKET_PORT 1020

typedef enum HmiCommandType HmiCommandType;
typedef struct HumanMachineInterfaceCommand HumanMachineInterfaceCommand;
typedef enum CentralEcuRequester CentralEcuRequester;

enum CentralEcuRequester
{
    HumanMachineInterfaceToCentralEcuRequester,
    FrontWindShieldCameraToCentralEcuRequester,
    ForwardFacingRadarToCentralEcuRequester,
    ParkAssistToCentralEcuRequester
};

enum HmiCommandType {
    Start,
    Parking,
    Stop
};


struct HumanMachineInterfaceCommand {
    HmiCommandType type;
};