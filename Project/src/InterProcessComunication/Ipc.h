#define DEFAULT_PROTOCOL 0

int createUnixSocket(int protocol);

int bindUnixSocket(int socketFd, const char *addressName);

int acceptUnixSocket(int socketFd);

int connectUnixSocket(int socketFd, const char *addressName);


int listenSocket(int socketFd, int queueLength);


void readRequest(int fd, int *requesterId, void **requestData, int *requestDataLengthInBytes);

void writeRequest(int fd, int requesterId, const void *requestData, int requestDataLengthInBytes);
