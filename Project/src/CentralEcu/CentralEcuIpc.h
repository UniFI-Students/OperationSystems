#ifndef OS_CENTRAL_ECU_H
#define OS_CENTRAL_ECU_H

#define CENTRAL_ECU_UNIX_SOCKET_NAME "cEcuSocket"
#define CENTRAL_ECU_INET_SOCKET_PORT 1020

enum CentralEcuRequester {
    HumanMachineInterfaceToCentralEcuRequester,
    FrontWindShieldCameraToCentralEcuRequester,
    ForwardFacingRadarToCentralEcuRequester,
    ParkAssistToCentralEcuRequester
} typedef CentralEcuRequester;

enum HmiCommandType {
    None,
    Start,
    Parking,
    Stop
} typedef HmiCommandType;


struct HumanMachineInterfaceCommand {
    HmiCommandType type;
} typedef HumanMachineInterfaceCommand;

#endif