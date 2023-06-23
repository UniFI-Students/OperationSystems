#include "Ipc.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <malloc.h>








int createUnixSocket(int protocol) {
    return socket(AF_UNIX, SOCK_STREAM, protocol);
}

int createInetSocket(int protocol) {
    return socket(AF_INET, SOCK_STREAM, protocol);
}

int bindUnixSocket(int socketFd, const char *addressName) {

    struct sockaddr_un serverUnixSocketAddress;
    struct sockaddr *serverUnixSocketAddrPtr = (struct sockaddr *) &serverUnixSocketAddress;
    socklen_t serverUnixSocketLen = sizeof(serverUnixSocketAddress);

    serverUnixSocketAddress.sun_family = AF_UNIX;
    strcpy(serverUnixSocketAddress.sun_path, addressName);

    return bind(socketFd, serverUnixSocketAddrPtr, serverUnixSocketLen);
}

int bindLocalInetSocket(int socketFd, int port){
    struct sockaddr_in serverInetSocketAddress;
    struct sockaddr *serverInetSocketAddrPtr = (struct sockaddr *) &serverInetSocketAddress;
    socklen_t serverInetSocketLen = sizeof(serverInetSocketAddress);

    serverInetSocketAddress.sin_family = AF_INET;
    serverInetSocketAddress.sin_port = htons(port);
    serverInetSocketAddress.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    return bind(socketFd, serverInetSocketAddrPtr, serverInetSocketLen);
}

int closeSocket(int socketFd){
    close(socketFd);
}
int listenSocket(int socketFd, int queueLength) {
    return listen(socketFd, queueLength);
}

int acceptUnixSocket(int socketFd) {
    struct sockaddr_un clientUnixSocketAddress;
    struct sockaddr *clientUnixSocketAddrPtr = (struct sockaddr *) &clientUnixSocketAddress;
    socklen_t clientUnixSocketLen = sizeof(clientUnixSocketAddress);

    return accept(socketFd, clientUnixSocketAddrPtr, &clientUnixSocketLen);
}

int acceptInetSocket(int socketFd){
    struct sockaddr_in clientInetSocketAddress;
    struct sockaddr *clientInetSocketAddrPtr = (struct sockaddr *) &clientInetSocketAddress;
    socklen_t clientInetSocketLen = sizeof(clientInetSocketAddress);

    return accept(socketFd, clientInetSocketAddrPtr, &clientInetSocketLen);
}

int connectUnixSocket(int socketFd, const char *addressName) {
    struct sockaddr_un serverUnixSocketAddress;
    struct sockaddr *serverUnixSocketAddrPtr = (struct sockaddr *) &serverUnixSocketAddress;
    socklen_t serverUnixSocketLen = sizeof(serverUnixSocketAddress);

    serverUnixSocketAddress.sun_family = AF_UNIX;
    strcpy(serverUnixSocketAddress.sun_path, addressName);

    return connect(socketFd, serverUnixSocketAddrPtr, serverUnixSocketLen);
}

int connectLocalInetSocket(int socketFd, int port){
    struct sockaddr_in serverInetSocketAddress;
    struct sockaddr *serverInetSocketAddrPtr = (struct sockaddr *) &serverInetSocketAddress;
    socklen_t serverInetSocketLen = sizeof(serverInetSocketAddress);

    serverInetSocketAddress.sin_family = AF_INET;
    serverInetSocketAddress.sin_port = htons(port);
    serverInetSocketAddress.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    return connect(socketFd, serverInetSocketAddrPtr, serverInetSocketLen);
}

int readRequest(int fd, int *requesterId, void **requestData, unsigned int *requestDataLengthInBytes) {
    read(fd, requesterId, sizeof(int));
    read(fd, requestDataLengthInBytes, sizeof(int));

    *requestData = malloc(*requestDataLengthInBytes);
    int bytesToRead = *requestDataLengthInBytes;
    while (bytesToRead != 0) {
        int bytesRead = (int) read(fd, *requestData, bytesToRead);
        *requestData += bytesRead;
        bytesToRead -= bytesRead;
    }
    *requestData -= *requestDataLengthInBytes;
    return 0;
}

int writeRequest(int fd, int requesterId, const void *requestData, unsigned int requestDataLengthInBytes) {
    write(fd, &requesterId, sizeof(int));
    write(fd, &requestDataLengthInBytes, sizeof(int));
    write(fd, requestData, requestDataLengthInBytes);
    return 0;
}