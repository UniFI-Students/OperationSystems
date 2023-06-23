#ifndef OS_CONSTS_H
#define OS_CONSTS_H

#define LOG_DIRECTORY "logs"

#define SIG_STOP_CAR SIGUSR1
#define SIG_THROTTLE_FAIL SIGUSR2

#define FAIL_PROBABILITY 1e-5

#define HUMAN_MACHINE_INTERFACE_EXE_FILENAME "hmi"
#define STEER_BY_WIRE_EXE_FILENAME "sbw"
#define THROTTLE_CONTROL_EXE_FILENAME "tc"
#define BRAKE_BY_WIRE_EXE_FILENAME "bbw"
#define FRONT_WIND_SHIELD_CAMERA_EXE_FILENAME "fwc"
#define FORWARD_FACING_RADAR_EXE_FILENAME "ffr"
#define PARK_ASSIST_EXE_FILENAME "pa"
#define SURROUND_VIEW_CAMERAS_EXE_FILENAME "svc"

#define CENTRAL_ECU_UNIX_SOCKET_NAME "cEcuSocket"
#define HUMAN_MACHINE_INTERFACE_UNIX_SOCKET_NAME "hmiSocket"
#define BREAK_BY_WIRE_UNIX_SOCKET_NAME "bbwSocketFd"
#define STEER_BY_WIRE_UNIX_SOCKET_NAME "sbwSocket"
#define THROTTLE_CONTROL_UNIX_SOCKET_NAME "tcSocket"

#define CENTRAL_ECU_INET_SOCKET_PORT 1020
#define HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT 1021
#define BRAKE_BY_WIRE_INET_SOCKET_PORT 1022
#define STEER_BY_WIRE_INET_SOCKET_PORT 1023
#define THROTTLE_CONTROL_INET_SOCKET_PORT 1024

#endif
