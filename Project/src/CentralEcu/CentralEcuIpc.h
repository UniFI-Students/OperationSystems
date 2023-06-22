#ifndef OS_CENTRAL_ECU_H
#define OS_CENTRAL_ECU_H

#define CENTRAL_ECU_UNIX_SOCKET_NAME "cEcuSocket"
#define CENTRAL_ECU_INET_SOCKET_PORT 1020

#define HUMAN_MACHINE_INTERFACE_EXE_FILENAME "hmi"
#define STEER_BY_WIRE_EXE_FILENAME "sbw"
#define THROTTLE_CONTROL_EXE_FILENAME "tc"
#define BREAK_BY_WIRE_EXE_FILENAME "bbw"
#define FRONT_WIND_SHIELD_CAMERA_EXE_FILENAME "fwsc"
#define FORWARD_FACING_RADAR_EXE_FILENAME "ffr"
#define PARK_ASSIST_EXE_FILENAME "pa"
#define SURROUND_VIEW_CAMERAS_EXE_FILENAME "svc"

enum CentralEcuRequester {
    HumanMachineInterfaceToCentralEcuRequester,
    FrontWindShieldCameraToCentralEcuRequester,
    ForwardFacingRadarToCentralEcuRequester,
    ParkAssistToCentralEcuRequester
} typedef CentralEcuRequester;

enum HmiCommandType {
    Start,
    Parking,
    Stop
} typedef HmiCommandType;


struct HumanMachineInterfaceCommand {
    HmiCommandType type;
} typedef HumanMachineInterfaceCommand;

#endif