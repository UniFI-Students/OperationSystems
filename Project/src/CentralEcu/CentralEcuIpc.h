#define CENTRAL_ECU_SOCKET_NAME "cEcu"

typedef enum CEcuExternalCommunicator CEcuExternalCommunicator;

enum CEcuExternalCommunicator
{
    HumanMachineInterface,
    FrontWindShieldCamera,
    ForwardFacingRadar,
    ParkAssist
};