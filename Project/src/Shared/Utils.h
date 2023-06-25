#ifndef OS_UTILS_H
#define OS_UTILS_H


void convertBytesToStringRepresentation(char *dest, const char *source, unsigned int size);
void createDirectoryIfDoesNotExist(const char *path);

void execEcuChildProcess(const char *childName);
void execEcuChildProcessWithIntArgument(const char *childName, int arg);
void execEcuChildProcessWithArgument(const char *childName, const char *arg);

#endif //OS_UTILS_H
