#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "Utils.h"
#include "../FilePathProvider/FilePathProvider.h"


void convertBytesToStringRepresentation(char *dest, const char *source, unsigned int size) {
    char convertedValueToHexString[16];
    for (int i=0; i<8; ++i){
        sprintf(convertedValueToHexString, "| 0x%.8X ", source[0]);
        strcat(dest, convertedValueToHexString);
    }
    strcat(dest, "|");
}

void createDirectoryIfDoesNotExist(const char* path) {
    struct stat dirStat;
    if (stat(path, &dirStat) < 0) {
        if (mkdir(path, S_IRWXO | S_IRWXG | S_IRWXU) != 0) perror(strerror(errno));
    }

}

void execEcuChildProcess(const char *childName) {
    char buff[128];
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, NULL);
    perror(strerror(errno));
    exit(-1);
}

void execEcuChildProcessWithArgument(const char *childName, const char *arg) {
    char buff[128];
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, arg, (char *) 0);
    perror(strerror(errno));
    exit(-1);
}

void execEcuChildProcessWithIntArgument(const char *childName, int arg) {
    char buff[128];
    char argStr[16];
    sprintf(argStr, "%d", arg);
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, argStr, (char *) 0);
    perror(strerror(errno));
    exit(-1);
}