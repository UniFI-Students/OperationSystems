#include "Bus.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct BusMessage BusMessage;

BusMessage* _messageQueue;
int _messageQueueSize;

ssize_t recieveBroadcastedMessage(int messageTag, const void *messageData)
{
    printf("Recieve message");
}
ssize_t recieveMessage(int senderTag, int messageTag, const void *messageData)
{
    printf("Recieve message");
}
ssize_t broadcastMessage(int messageTag, const void *messageData)
{
    printf("Recieve message");
}
ssize_t sendMessage(int recieverTag, int messageTag, const void *messageData)
{
    
    printf("Recieve message");
}

ssize_t hasMessage(int messageTag)
{
    printf("Recieve message");
}
ssize_t waitForMessage(int messageTag)
{
    printf("Recieve message");
}

ssize_t connectToBus()
{
    printf("Recieve message");
}
ssize_t createBus(int messageQueueSize)
{
    _messageQueueSize = messageQueueSize;
    _messageQueue = calloc(_messageQueueSize, sizeof(BusMessage));
    
}


int main()
{
    createBus(0);
}