#ifndef HUMAN_MACHINE_INTERFACE_IPC_H
#define HUMAN_MACHINE_INTERFACE_IPC_H

#define HUMAN_MACHINE_INTERFACE_UNIX_SOCKET_NAME "hmiSocket"
#define HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT 1021

typedef enum HmiRequester HmiRequester;

enum HmiRequester
{
    CentralEcuToHmiRequester
};

#endif
