#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "Utils.h"


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