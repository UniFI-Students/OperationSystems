#ifndef OS_CONSTS_H
#define OS_CONSTS_H

#define LOG_DIRECTORY "logs"
#define ERROR_LOG_DIRECTORY "eLogs"

#define SIG_STOP_CAR SIGUSR1
#define SIG_THROTTLE_FAIL SIGUSR2

#define READ_PIPE 0
#define WRITE_PIPE 1

#define FAIL_PROBABILITY 1e-5

#define DEFAULT_THROTTLE_QUANTITY 5
#define DEFAULT_BRAKE_QUANTITY 5

#define TIME_NEEDED_TO_PARK 30


#define FWC_STEER_RIGHT_MESSAGE "DESTRA"
#define FWC_STEER_LEFT_MESSAGE "SINISTRA"
#define FWC_DANGER_MESSAGE "PERICOLO"
#define FWC_PARK_CAR_MESSAGE "PARCHEGGIO"

#define PA_REQUEST_FAILED_VALUES_COUNT 6
#define PA_REQUEST_FAILED_VALUES { 0x172A, 0xD693, 0x0000, 0xBDD8, 0xFAEE, 0x4300}



#define NORMAL_EXECUTION_RANDOM_DATASOURCE "/dev/urandom"
#define ARTIFICIAL_EXECUTION_RANDOM_DATASOURCE "urandomARTIFICIALE.binary"

#define NORMAL_EXECUTION_EXTERNAL_ARGUMENT_NAME "NORMALE"
#define ARTIFICIAL_EXECUTION_EXTERNAL_ARGUMENT_NAME "ARTIFICIALE"
#define NORMAL_EXECUTION "-n"
#define ARTIFICIAL_EXECUTION "-a"

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
#define PARK_ASSIST_UNIX_SOCKET_NAME "paSocket"

#define CENTRAL_ECU_INET_SOCKET_PORT 1020
#define HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT 1021
#define BRAKE_BY_WIRE_INET_SOCKET_PORT 1022
#define STEER_BY_WIRE_INET_SOCKET_PORT 1023
#define THROTTLE_CONTROL_INET_SOCKET_PORT 1024
#define PARK_ASSIST_INET_SOCKET_PORT 1025

#endif
