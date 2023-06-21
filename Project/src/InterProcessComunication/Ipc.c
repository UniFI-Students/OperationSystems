#include "Ipc.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <malloc.h>


struct sockaddr_un serverAddress;
struct sockaddr* serverSockAddrPtr = (struct sockaddr*) &serverAddress;
socklen_t serverLen = sizeof(serverAddress);

struct sockaddr_un clientAddress;
struct sockaddr* clientSockAddrPtr = (struct sockaddr*) &clientAddress;
socklen_t clientLen = sizeof(clientAddress);

int createUnixSocket(int protocol)
{
    return socket(AF_UNIX, SOCK_DGRAM, protocol);
}
int bindUnixSocket(int socketFd, const char *addressName)
{
    serverAddress.sun_family = AF_UNIX;
    strcpy(serverAddress.sun_path, addressName);

    return bind(socketFd, serverSockAddrPtr, serverLen);
}

int listenSocket(int socketFd, int queueLength)
{
    return listen(socketFd, queueLength);
}
int acceptUnixSocket(int socketFd)
{
    return accept(socketFd, clientSockAddrPtr, &clientLen);
}
int connectUnixSocket(int socketFd, const char *addressName){
    serverAddress.sun_family = AF_UNIX;
    strcpy(serverAddress.sun_path, addressName);

    return connect(socketFd, serverSockAddrPtr, serverLen);
}

void readRequest(int fd, int *requesterId, void **requestData, int *requestDataLengthInBytes)
{
    read(fd, requesterId, sizeof(int));
    read(fd, requestDataLengthInBytes, sizeof(int));

    *requestData = malloc(*requestDataLengthInBytes);
    int bytesToRead = *requestDataLengthInBytes;
    while (bytesToRead!=0)
    {
        int bytesRead = (int)read(fd, *requestData, bytesToRead);
        *requestData += bytesRead;
        bytesToRead -= bytesRead;
    }
    *requestData -= *requestDataLengthInBytes;
}

void writeRequest(int fd, int requesterId, const void *requestData, int requestDataLengthInBytes)
{
    write(fd, &requesterId, sizeof(int));
    write(fd, &requestDataLengthInBytes, sizeof(int));
    write(fd, requestData, requestDataLengthInBytes);
}