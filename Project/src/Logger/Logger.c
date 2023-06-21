#include "Logger.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define LOG_DIRECTORY "log"

char logFilePath[64];
char errorLogFilePath[64];

void setLogFileName(const char *logFileName){

    if (getcwd(logFilePath, sizeof(logFilePath)) == NULL) {
        perror(strerror(errno));
        exit(-1);
    }
    strcat(logFilePath, "/");
    strcat(logFilePath, LOG_DIRECTORY);
    strcat(logFilePath, "/");
    strcat(logFilePath, logFileName);
}
void setErrorLogFileName(const char *errorLogFileName)
{
    if (getcwd(errorLogFilePath, sizeof(logFilePath)) == NULL) {
        perror(strerror(errno));
        exit(-1);
    }
    strcat(errorLogFilePath, "/");
    strcat(errorLogFilePath, LOG_DIRECTORY);
    strcat(errorLogFilePath, "/");
    strcat(errorLogFilePath, errorLogFileName);
}

void logMessage(const char *message){
    //TODO: Implement logMessage
}

void logLastError(){
    perror(strerror(errno));
    errno = 0;
}

void logError(const char *message){
    perror(message);
}