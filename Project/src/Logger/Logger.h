#ifndef LOGGER_H
#define LOGGER_H

void setLogFileName(const char *logFileName);
void setErrorLogFileName(const char *errorLogFileName);

void instantiateLogFileDescriptor();
void instantiateErrorLogFileDescriptor();
void closeLogFileDescriptor();
void closeErrorLogFileDescriptor();

void logMessage(const char *message);
void logLastError();
void logLastErrorWithMessage(const char *message);

#endif