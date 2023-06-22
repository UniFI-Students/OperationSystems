#ifndef INTER_PROCESS_COMMUNICATION_H
#define INTER_PROCESS_COMMUNICATION_H

#define DEFAULT_PROTOCOL 0

int createUnixSocket(int protocol);
int createInetSocket(int protocol);

int bindUnixSocket(int socketFd, const char *addressName);
int bindLocalInetSocket(int socketFd, int port);

int acceptUnixSocket(int socketFd);
int acceptInetSocket(int socketFd);

int connectUnixSocket(int socketFd, const char *addressName);
int connectLocalInetSocket(int socketFd, int port);


int listenSocket(int socketFd, int queueLength);
int closeSocket(int socketFd);


int readRequest(int fd, int *requesterId, void **requestData, int *requestDataLengthInBytes);

int writeRequest(int fd, int requesterId, const void *requestData, int requestDataLengthInBytes);

#endif