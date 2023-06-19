#include <sys/types.h>

ssize_t recieveBroadcastedMessage(int messageTag, const void *messageData);
ssize_t recieveMessage(int senderTag, int messageTag, const void *messageData);
ssize_t broadcastMessage(int messageTag, const void *messageData);
ssize_t sendMessage(int recieverTag, int messageTag, const void *messageData);

ssize_t hasMessage(int messageTag);
ssize_t waitForMessage(int messageTag);

ssize_t connectToBus();
ssize_t createBus(int messageQueueSize);

struct BusMessage
{
    int Tag;
    const void *Data;
    const void *DataSize;
};