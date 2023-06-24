
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
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

#define CENTRAL_ECU_LOGFILE "ECU.log"
#define CENTRAL_ECU_ERROR_LOGFILE "ECU.eLog"

enum CarState {
    CarStateNone,
    CarStateStarted,
    CarStateParking
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

int centralEcuSpeedCorrectorPid;

int speed;
int desiredSpeed;
CarState carState;

char executionType[3];

int pipeBetweenCEcuSpeedCorrectorAndCEcu[2];


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


void parkCar();


void execEcuChildProcess(const char *childName);

void execEcuChildProcessWithIntArgument(const char *childName, int arg);

void execEcuChildProcessWithArgument(const char *childName, const char *arg);


void closeChildProcesses();

void adjustSpeedToDesiredSpeed();

void handleFwcIntegerData(int data);

void stopCar();

void steer(enum SteerByWireCommandType type);

void handleAlarmSignal();

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

    acceptedSocket = -1;
    cEcuSocketFd = -1;
    pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE] = -1;
    pipeBetweenCEcuSpeedCorrectorAndCEcu[WRITE_PIPE] = -1;

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
    alarm(1);

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
    if (pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE] >= 0){
        read(pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE], &speed, sizeof(speed));
    }
    if (speed != desiredSpeed) adjustSpeedToDesiredSpeed();
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
}

void handleParkingCommandFromHmi() {
    if (carState != CarStateStarted) return;
    parkCar();

}

void handleStopCommandFromHmi() {
    if (carState != CarStateStarted) return;
    stopCar();
}

void stopCar() {
    speed = 0;
    sendStopSignalToBbw(brakeByWirePid);
    sendMessageToHmi("Sending stop signal to brake-by-wire.");
}


void runParkingSensors() {
    parkAssistPid = fork();
    if (parkAssistPid == 0) execEcuChildProcessWithArgument(PARK_ASSIST_EXE_FILENAME, executionType);
    surroundViewCamerasPid = fork();
    if (surroundViewCamerasPid == 0) execEcuChildProcessWithArgument(SURROUND_VIEW_CAMERAS_EXE_FILENAME, executionType);
}

void stopParkingSensors() {
    if (parkAssistPid != 0) kill(parkAssistPid, SIGKILL);
    if (surroundViewCamerasPid != 0) kill(surroundViewCamerasPid, SIGKILL);
    parkAssistPid = 0;
    surroundViewCamerasPid = 0;
}


void runSensors() {
    frontWindShieldCameraPid = fork();
    if (frontWindShieldCameraPid == 0) execEcuChildProcess(FRONT_WIND_SHIELD_CAMERA_EXE_FILENAME);

    forwardFacingRadarPid = fork();
    if (forwardFacingRadarPid == 0) execEcuChildProcessWithArgument(FORWARD_FACING_RADAR_EXE_FILENAME, executionType);
}


void stopSensors() {
    if (frontWindShieldCameraPid != 0) kill(frontWindShieldCameraPid, SIGKILL);
    if (forwardFacingRadarPid != 0) kill(forwardFacingRadarPid, SIGKILL);
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
    if (steerByWirePid != 0) kill(steerByWirePid, SIGKILL);
    if (throttleControlPid != 0) kill(throttleControlPid, SIGKILL);
    if (brakeByWirePid != 0) kill(brakeByWirePid, SIGKILL);
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


void parkCar() {
    //TODO: Implement parkCar
    carState = CarStateParking;
    printf("PARKING CAR");
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
        parkCar();
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
    if (centralEcuSpeedCorrectorPid != 0) {
        kill(centralEcuSpeedCorrectorPid, SIGKILL);
        centralEcuSpeedCorrectorPid = 0;
        close(pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE]);
        close(pipeBetweenCEcuSpeedCorrectorAndCEcu[WRITE_PIPE]);
        pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE] = -1;
        pipeBetweenCEcuSpeedCorrectorAndCEcu[WRITE_PIPE] = -1;
    }

    pipe(pipeBetweenCEcuSpeedCorrectorAndCEcu);

    centralEcuSpeedCorrectorPid = fork();
    if (centralEcuSpeedCorrectorPid == 0) {
        close(pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE]);
        pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE] = -1;
        closeSocket(acceptedSocket);
        acceptedSocket = -1;
        closeSocket(cEcuSocketFd);
        cEcuSocketFd = -1;

        while (speed < desiredSpeed) {
            int throttleValue = DEFAULT_THROTTLE_QUANTITY;
            if (speed + throttleValue > desiredSpeed) throttleValue = desiredSpeed - speed;
            speed += throttleValue;

            write(pipeBetweenCEcuSpeedCorrectorAndCEcu[WRITE_PIPE], &speed, sizeof(speed));
            sendThrottleRequestToTc(throttleValue);
            sendMessageToHmi("Sending throttle request to tc.");
            sleep(1);
        }

        while (speed > desiredSpeed) {
            int brakeValue = DEFAULT_BRAKE_QUANTITY;
            if (speed - brakeValue < desiredSpeed) brakeValue = speed - desiredSpeed;
            speed -= brakeValue;

            write(pipeBetweenCEcuSpeedCorrectorAndCEcu[WRITE_PIPE], &speed, sizeof(speed));
            sendBrakeRequestToBbw(brakeValue);
            sendMessageToHmi("Sending brake request to bbw.");
            sleep(1);
        }

        closeFileDescriptors();
        exit(0);
    }
    close(pipeBetweenCEcuSpeedCorrectorAndCEcu[WRITE_PIPE]);
    pipeBetweenCEcuSpeedCorrectorAndCEcu[WRITE_PIPE] = -1;

    fcntl( pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE], F_SETFL, fcntl(pipeBetweenCEcuSpeedCorrectorAndCEcu[READ_PIPE], F_GETFL) | O_NONBLOCK);
}

void handleFfrRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (carState != CarStateStarted) return;
}


void handlePaRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (!carState) return;
    printf("Pa REQUEST\n");
}
