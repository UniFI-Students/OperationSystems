#include "Logger.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "../DateProvider/DateProvider.h"
#include "../Shared/Consts.h"
#include "../Shared/Utils.h"


char logFilePath[128];
char errorLogFilePath[128];
FILE* logFdPtr;
FILE* errorLogFdPtr;




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
    if (getcwd(errorLogFilePath, sizeof(errorLogFilePath)) == NULL) {
        perror(strerror(errno));
        exit(-1);
    }
    strcat(errorLogFilePath, "/");
    strcat(errorLogFilePath, ERROR_LOG_DIRECTORY);

    createDirectoryIfDoesNotExist(errorLogFilePath);

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
    fprintf(logFdPtr, "%s\n", message);
    fflush(logFdPtr);
}

void logLastError(){
    perror();
    fprintf(errorLogFdPtr, "%s: Error (%d) - %s\n", getCurrentDateTime(), errno, strerror(errno));
    fflush(errorLogFdPtr);
    errno = 0;
}

void logLastErrorWithWhenMessage(const char *whenMessage){
    perror(whenMessage);
    fprintf(errorLogFdPtr, "%s: Error (%d) - %s when %s\n", getCurrentDateTime(), errno, strerror(errno), whenMessage);
    fflush(errorLogFdPtr);
}

void logErrorMessage(const char *message){
    fprintf(errorLogFdPtr, "%s: Error happened - %s\n", getCurrentDateTime(), message);
    fflush(errorLogFdPtr);
}

