
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include "CentralEcuIpc.h"
#include "CentralEcuBbwIpc/CentralEcuBbwIpc.h"
#include "CentralEcuHmiIpc/CentralEcuHmiIpc.h"
#include "CentralEcuSbwIpc/CentralEcuSbwIpc.h"
#include "CentralEcuTcIpc/CentralEcuTcIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../HumanMachineInterface/HumanMachineInterfaceIpc.h"
#include "../Shared/Consts.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../ParkAssist/ParkAssistIpc.h"

#define CENTRAL_ECU_LOGFILE "ECU.log"
#define CENTRAL_ECU_ERROR_LOGFILE "ECU.eLog"

enum CarState {
    CarStateNone = 1 << 0,
    CarStateStarted = 1 << 1,
    CarStatePreparingToPark = 1 << 2,
    CarStateParking = 1 << 3
} typedef CarState;

int cEcuSocketFd;
int acceptedSocket;


int steerByWirePid;
int throttleControlPid;
int brakeByWirePid;

int frontWindShieldCameraPid;
int forwardFacingRadarPid;

int parkAssistPid;
int surroundViewCamerasPid;

int speed;
int desiredSpeed;
CarState carState;

char executionType[3];


int remainingSecondsForParkAssist;

bool hasReceivedBadValue;


void registerSignalHandlers();

void handleThrottleFailSignal();

void handleInterruptSignal();

void terminateProgramExecution(int status);

void handleHmiRequest(const void *requestDataPtr, unsigned int requestDataLength);

void handlePaRequest(void *requestDataPtr, unsigned int requestDataLength);

void handleFwcRequest(void *requestDataPtr, unsigned int requestDataLength);

void handleFfrRequest(void *requestDataPtr, unsigned int requestDataLength);


void handleStartCommandFromHmi();

void handleParkingCommandFromHmi();

void handleStopCommandFromHmi();

void closeFileDescriptors();

void runActuators();

void stopActuators();

void runSensors();

void stopSensors();

void runParkingSensors();

void stopParkingSensors();


void initiateParking();


void execEcuChildProcess(const char *childName);

void execEcuChildProcessWithIntArgument(const char *childName, int arg);

void execEcuChildProcessWithArgument(const char *childName, const char *arg);


void closeChildProcesses();

void adjustSpeedToDesiredSpeed();

void handleFwcIntegerData(int data);

void stopCar();

void steer(enum SteerByWireCommandType type);

void handleAlarmSignal();

void parkCar();

void activateParkAssist();

void handleSuccessfulParking();


bool hasPaRequestBadValue(void *requestData, unsigned int dataLength);

void reParkCar();

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        logLastErrorWithMessage("Unassigned execution type argument.");
        exit(-1);
    }

    if (strcmp(argv[1], NORMAL_EXECUTION_EXTERNAL_ARGUMENT_NAME) == 0) strcpy(executionType, NORMAL_EXECUTION);
    if (strcmp(argv[1], ARTIFICIAL_EXECUTION_EXTERNAL_ARGUMENT_NAME) == 0) strcpy(executionType, ARTIFICIAL_EXECUTION);
    if (strlen(executionType) == 0) {
        logLastErrorWithMessage("Incorrect execution type argument.");
        exit(-1);
    }

    cEcuSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    speed = 0;
    carState = CarStateNone;

    setLogFileName(CENTRAL_ECU_LOGFILE);
    setErrorLogFileName(CENTRAL_ECU_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    if (cEcuSocketFd < 0) {
        logLastError();
        terminateProgramExecution(-1);
    }
    if (bindLocalInetSocket(cEcuSocketFd, CENTRAL_ECU_INET_SOCKET_PORT) < 0) {
        logLastError();
        terminateProgramExecution(-1);
    }
    if (listenSocket(cEcuSocketFd, 5) < 0) {
        logLastError();
        terminateProgramExecution(-1);
    }


    registerSignalHandlers();

    while (1) {
        acceptedSocket = acceptInetSocket(cEcuSocketFd);
        if (acceptedSocket < 0) {
            logLastError();
            terminateProgramExecution(-1);
        }

        int requesterId;
        void *requestDataPtr;
        unsigned int requestDataLength;

        if (readRequest(acceptedSocket, &requesterId, &requestDataPtr, &requestDataLength) < 0) logLastError();

        switch ((CentralEcuRequester) requesterId) {
            case HumanMachineInterfaceToCentralEcuRequester:
                handleHmiRequest(requestDataPtr, requestDataLength);
                break;
            case FrontWindShieldCameraToCentralEcuRequester:
                handleFwcRequest(requestDataPtr, requestDataLength);
                break;
            case ForwardFacingRadarToCentralEcuRequester:
                handleFfrRequest(requestDataPtr, requestDataLength);
                break;
            case ParkAssistToCentralEcuRequester:
                handlePaRequest(requestDataPtr, requestDataLength);
                break;
            default:
                logLastErrorWithMessage("Unknown request arrived.");
                break;
        }
        free(requestDataPtr);

        if (closeSocket(acceptedSocket) < 0) {
            logLastError();
            terminateProgramExecution(-1);
        }
    }

}

void closeFileDescriptors() {
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();
    closeSocket(cEcuSocketFd);
    closeSocket(acceptedSocket);
}


void registerSignalHandlers() {
    signal(SIGCHLD, SIG_IGN);
    signal(SIG_THROTTLE_FAIL, handleThrottleFailSignal);
    signal(SIGINT, handleInterruptSignal);
    signal(SIGALRM, handleAlarmSignal);
}


void handleThrottleFailSignal() {
    speed = 0;
    desiredSpeed = 0;
    sendMessageToHmi("Throttle failed. Terminating current execution.");
    stopSensors();
    stopParkingSensors();
    stopActuators();
    carState = CarStateNone;
}

void handleInterruptSignal() {
    terminateProgramExecution(0);
}

void handleAlarmSignal() {
    if (carState == CarStatePreparingToPark && speed == 0) {
        parkCar();
    }
    if (carState & (CarStateStarted | CarStatePreparingToPark)) adjustSpeedToDesiredSpeed();
    if (remainingSecondsForParkAssist > 0) {
        --remainingSecondsForParkAssist;
        if (remainingSecondsForParkAssist == 0) {
            if (hasReceivedBadValue) {
                reParkCar();
                return;
            }
            sendMessageToHmi("CAR IS PARKED");
            //handleSuccessfulParking();
        }
    }
    alarm(1);
}




void terminateProgramExecution(int status) {
    closeFileDescriptors();
    closeChildProcesses();
    exit(status);
}

void closeChildProcesses() {
    stopActuators();
    stopParkingSensors();
    stopSensors();
}

void handleHmiRequest(const void *requestDataPtr, unsigned int requestDataLength) {
    if (carState & (CarStatePreparingToPark | CarStateParking)) return;
    HumanMachineInterfaceCommand *cmdPtr = (HumanMachineInterfaceCommand *) requestDataPtr;
    switch (cmdPtr->type) {

        case Start:
            handleStartCommandFromHmi();
            break;
        case Parking:
            handleParkingCommandFromHmi();
            break;
        case Stop:
            handleStopCommandFromHmi();
            break;
        case None:
            logLastErrorWithMessage("Invalid hmi command type");
            break;
    }

}

void handleStartCommandFromHmi() {
    if (carState != CarStateNone) return;
    carState = CarStateStarted;
    runActuators();
    runSensors();
    alarm(1);
}

void handleParkingCommandFromHmi() {
    if (carState != CarStateStarted) return;
    initiateParking();

}

void handleStopCommandFromHmi() {
    if (carState != CarStateStarted) return;
    stopCar();
}

void stopCar() {
    speed = 0;
    desiredSpeed = 0;
    sendStopSignalToBbw(brakeByWirePid);
    sendMessageToHmi("Sending stop signal to brake-by-wire.");
}


void runParkingSensors() {
    parkAssistPid = fork();
    if (parkAssistPid == 0) execEcuChildProcessWithArgument(PARK_ASSIST_EXE_FILENAME, executionType);
}

void stopParkingSensors() {
    if (parkAssistPid != 0) kill(parkAssistPid, SIGINT);
    parkAssistPid = 0;
}


void runSensors() {
    frontWindShieldCameraPid = fork();
    if (frontWindShieldCameraPid == 0) execEcuChildProcess(FRONT_WIND_SHIELD_CAMERA_EXE_FILENAME);

    forwardFacingRadarPid = fork();
    if (forwardFacingRadarPid == 0) execEcuChildProcessWithArgument(FORWARD_FACING_RADAR_EXE_FILENAME, executionType);
}


void stopSensors() {
    if (frontWindShieldCameraPid != 0) kill(frontWindShieldCameraPid, SIGINT);
    if (forwardFacingRadarPid != 0) kill(forwardFacingRadarPid, SIGINT);
    frontWindShieldCameraPid = 0;
    forwardFacingRadarPid = 0;
}

void runActuators() {
    int cEcuPid = getpid();
    steerByWirePid = fork();
    if (steerByWirePid == 0) execEcuChildProcess(STEER_BY_WIRE_EXE_FILENAME);
    throttleControlPid = fork();
    if (throttleControlPid == 0) execEcuChildProcessWithIntArgument(THROTTLE_CONTROL_EXE_FILENAME, cEcuPid);
    brakeByWirePid = fork();
    if (brakeByWirePid == 0) execEcuChildProcess(BRAKE_BY_WIRE_EXE_FILENAME);
}


void stopActuators() {
    if (steerByWirePid != 0) kill(steerByWirePid, SIGINT);
    if (throttleControlPid != 0) kill(throttleControlPid, SIGINT);
    if (brakeByWirePid != 0) kill(brakeByWirePid, SIGINT);
    steerByWirePid = 0;
    throttleControlPid = 0;
    brakeByWirePid = 0;
}

void execEcuChildProcess(const char *childName) {
    closeFileDescriptors();
    char buff[128];
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, NULL);
    logLastError();
    exit(-1);
}

void execEcuChildProcessWithArgument(const char *childName, const char *arg) {
    closeFileDescriptors();
    char buff[128];
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, arg, (char *) 0);
    logLastError();
    exit(-1);
}

void execEcuChildProcessWithIntArgument(const char *childName, int arg) {
    closeFileDescriptors();
    char buff[128];
    char argStr[16];
    sprintf(argStr, "%d", arg);
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, argStr, (char *) 0);
    logLastError();
    exit(-1);
}


void initiateParking() {
    carState = CarStatePreparingToPark;
    desiredSpeed = 0;
    runParkingSensors();
}


void handleFwcRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (carState != CarStateStarted) return;

    if (strcmp(requestDataPtr, FWC_STEER_LEFT_MESSAGE) == 0) {
        steer(Left);
        return;
    }

    if (strcmp(requestDataPtr, FWC_STEER_RIGHT_MESSAGE) == 0) {
        steer(Right);
        return;
    }

    if (strcmp(requestDataPtr, FWC_DANGER_MESSAGE) == 0) {
        stopCar();
        return;
    }

    if (strcmp(requestDataPtr, FWC_PARK_CAR_MESSAGE) == 0) {
        initiateParking();
        return;
    }

    errno = 0;
    int integerFromRequest = (int) strtol(requestDataPtr, requestDataPtr + requestDataLength, 10);
    if (errno == 0) {
        handleFwcIntegerData(integerFromRequest);
        return;
    }
    errno = 0;
    logLastErrorWithMessage("Incorrect request from fwc.");
}

void steer(SteerByWireCommandType type) {
    sendSteerRequestToSbw(type);
    sendMessageToHmi("Sending steer right request to sbw.");

}

void handleFwcIntegerData(int data) {
    if (desiredSpeed != data) {
        desiredSpeed = data;
    }
}

void adjustSpeedToDesiredSpeed() {

    if (speed < desiredSpeed) {
        int throttleValue = DEFAULT_THROTTLE_QUANTITY;
        if (speed + throttleValue > desiredSpeed) throttleValue = desiredSpeed - speed;
        speed += throttleValue;

        sendThrottleRequestToTc(throttleValue);
        sendMessageToHmi("Sending throttle request to tc.");
    } else if (speed > desiredSpeed) {
        int brakeValue = DEFAULT_BRAKE_QUANTITY;
        if (speed - brakeValue < desiredSpeed) brakeValue = speed - desiredSpeed;
        speed -= brakeValue;

        sendBrakeRequestToBbw(brakeValue);
        sendMessageToHmi("Sending brake request to bbw.");
    }
}

void handleFfrRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (carState != CarStateStarted) return;
}


void parkCar() {

    stopSensors();
    stopActuators();


    remainingSecondsForParkAssist = 2;
    hasReceivedBadValue = false;
    carState = CarStateParking;
    activateParkAssist();
    sendMessageToHmi("Activating Park assist.");
}

void reParkCar() {
    remainingSecondsForParkAssist = 2;
    hasReceivedBadValue = false;
    carState = CarStateParking;
    activateParkAssist();
    sendMessageToHmi("Reactivating Park assist.");
}


void handlePaRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (carState != CarStateParking) return;
    hasReceivedBadValue |= hasPaRequestBadValue(requestDataPtr, requestDataLength);
}

bool hasPaRequestBadValue(void *requestData, unsigned int dataLength) {
    int failedValue[PA_REQUEST_FAILED_VALUES_COUNT] = PA_REQUEST_FAILED_VALUES;
    for (int i = 0; i < PA_REQUEST_FAILED_VALUES_COUNT; ++i) {
        if (*(int *) requestData == failedValue[i]) return true;
    }
    return false;
}

void handleSuccessfulParking() {
    carState = CarStateNone;
    stopParkingSensors();
    sendMessageToHmi("Car were successfully parked.");
}

void activateParkAssist() {
    int paSocket = createInetSocket(DEFAULT_PROTOCOL);
    if (paSocket < 0) {
        logLastError();
        return;
    }
    int paConnectionRes = connectLocalInetSocket(paSocket, PARK_ASSIST_INET_SOCKET_PORT);
    while (paConnectionRes < 0) {
        logLastErrorWithMessage("Trying to connect to pa.");
        paConnectionRes = connectLocalInetSocket(paSocket, PARK_ASSIST_INET_SOCKET_PORT);
        sleep(1);
    }

    if (writeRequest(paSocket, CentralEcuToParkAssistRequester, NULL, 0)) {
        logLastError();
    }
    closeSocket(paSocket);
}
