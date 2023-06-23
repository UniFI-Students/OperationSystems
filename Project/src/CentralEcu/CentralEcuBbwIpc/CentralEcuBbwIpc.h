#ifndef OS_CENTRAL_ECU_BRAKE_BY_WIRE_IPC_H
#define OS_CENTRAL_ECU_BRAKE_BY_WIRE_IPC_H




void sendStopSignalToBbw(int brakeByWirePid);
void sendBrakeRequestToBbw(int brakeQuantity);


#endif
