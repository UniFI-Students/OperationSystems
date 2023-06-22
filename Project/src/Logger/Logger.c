#include "Logger.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../DateProvider/DateProvider.h"

#define LOG_DIRECTORY "log"

char logFilePath[128];
char errorLogFilePath[128];
FILE* logFdPtr;
FILE* errorLogFdPtr;

void createDirectoryIfDoesNotExist(const char *path);


void setLogFileName(const char *logFileName){

    if (getcwd(logFilePath, sizeof(logFilePath)) == NULL) {
        perror(strerror(errno));
        exit(-1);
    }
    strcat(logFilePath, "/");
    strcat(logFilePath, LOG_DIRECTORY);

    createDirectoryIfDoesNotExist(logFilePath);

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

    createDirectoryIfDoesNotExist(logFilePath);

    strcat(errorLogFilePath, "/");
    strcat(errorLogFilePath, errorLogFileName);
}

void instantiateLogFileDescriptor(){
    logFdPtr = fopen(logFilePath, "a");
}
void instantiateErrorLogFileDescriptor(){
    errorLogFdPtr = fopen(errorLogFilePath, "a");
}
void closeLogFileDescriptor(){
    fclose(logFdPtr);
}
void closeErrorLogFileDescriptor(){
    fclose(errorLogFdPtr);
}

void logMessage(const char *message){
    fprintf(logFdPtr, "%s", message);
}

void logLastError(){
    perror(strerror(errno));
    fprintf(errorLogFdPtr, "%s: Error (%d) - %s\n", getCurrentDateTime(), errno, strerror(errno));
    errno = 0;
}

void logError(const char *message){
    perror(message);
    fprintf(errorLogFdPtr, "%s: Error (%d) - %s\n", getCurrentDateTime(), errno, strerror(errno));
}

void createDirectoryIfDoesNotExist(const char* path) {
    struct stat dirStat;
    if (stat(path, &dirStat) < 0) mkdir(path, 666);

}