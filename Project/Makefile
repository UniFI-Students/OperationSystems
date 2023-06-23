CC = gcc
CFLAGS =
C_FILES = $(shell find src -name '*.c')

#			OBJ FILE DEPENDENCIES				#

#HUMAN MACHINE INTERFACE
HMI_OBJ_FILES = build/HumanMachineInterface.o 	\
				build/Ipc.o						\
				build/Logger.o 					\
				build/DateProvider.o

#STEER BY WIRE
SBW_OBJ_FILES =	build/SteerByWire.o 			\
				build/Ipc.o						\
				build/Logger.o 					\
				build/DateProvider.o

#THROTTLE CONTROL
TC_OBJ_FILES = 	build/ThrottleControl.o 		\
				build/Ipc.o						\
				build/Logger.o 					\
				build/Random.o 					\
				build/DateProvider.o

#BRAKE BY WIRE
BBW_OBJ_FILES = build/BrakeByWire.o 			\
				build/Ipc.o						\
				build/Logger.o 					\
				build/DateProvider.o

#FRONT WIND SHIELD CAMERA
FWC_OBJ_FILES = build/FrontWindshieldCamera.o	\
 				build/Logger.o 					\
 				build/DateProvider.o

#FORWARD FACING RADAR
FFR_OBJ_FILES = build/ForwardFacingRadar.o 		\
				build/Logger.o 					\
				build/DateProvider.o

#PARK ASSIST
PA_OBJ_FILES = build/ParkAssist.o 				\
				build/Logger.o 					\
				build/DateProvider.o

#SURROUND VIEW CAMERAS
SVC_OBJ_FILES = build/SurroundViewCameras.o 	\
				build/Logger.o 					\
				build/DateProvider.o

#CENTRAL ECU
ECU_OBJ_FILES = build/CentralEcu.o 				\
				build/Ipc.o 					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/CentralEcuHmiIpc.o		\
				build/CentralEcuBbwIpc.o		\
				build/CentralEcuSbwIpc.o		\
				build/CentralEcuTcIpc.o

#			END OBJ FILE DEPENDENCIES			#

DEBUG ?= 1

ifeq ($(DEBUG), 1)
    CFLAGS = -g
else
    CFLAGS =
endif

all: buildFileObjects buildApp

buildApp: $(HMI_OBJ_FILES)
	$(AT) mkdir -p app
	$(CC) $(HMI_OBJ_FILES)  -o app/hmi $(CFLAGS)
	$(CC) $(SBW_OBJ_FILES)  -o app/sbw $(CFLAGS)
	$(CC) $(TC_OBJ_FILES)  -o app/tc $(CFLAGS)
	$(CC) $(BBW_OBJ_FILES)  -o app/bbw $(CFLAGS)
	$(CC) $(FWC_OBJ_FILES)  -o app/fwc $(CFLAGS)
	$(CC) $(FFR_OBJ_FILES)  -o app/ffr $(CFLAGS)
	$(CC) $(PA_OBJ_FILES)  -o app/pa $(CFLAGS)
	$(CC) $(SVC_OBJ_FILES)  -o app/svc $(CFLAGS)
	$(CC) $(ECU_OBJ_FILES)  -o app/ecu $(CFLAGS)

buildFileObjects: $(C_FILES)
	$(AT) mkdir -p build
	$(foreach file, $^, ${CC} -c $(file) -o $(addprefix build/,$(notdir $(file:%.c=%.o))) $(CFLAGS);)

clean:
	@echo Removing application
	$(AT)-rm -rf app
	@echo Removing build
	$(AT)-rm -rf build

cleanLogs:
	$(AT)-rm -rf app/logs