#ifndef OS_CENTRAL_ECU_H
#define OS_CENTRAL_ECU_H





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