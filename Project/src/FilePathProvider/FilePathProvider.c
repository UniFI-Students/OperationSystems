#include <unistd.h>
#include <string.h>
#include "FilePathProvider.h"

void getCwdWithFileName(const char *fileName, char *buff, int size) {
    getcwd(buff, size);
    strcat(buff, "/");
    strcat(buff, fileName);
}