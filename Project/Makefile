CC = gcc
CFLAGS =
C_FILES = $(shell find src -name '*.c')

NEED_FILES_IN_APP_FROM_SRC = 	src/frontCamera.data			\
								src/urandomARTIFICIALE.binary	\
								src/run.sh

#			OBJ FILE DEPENDENCIES				#

#HUMAN MACHINE INTERFACE
HMI_OBJ_FILES = build/HumanMachineInterface.o 	\
				build/Ipc.o						\
				build/Utils.o					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/CentralEcuIpc.o			\
				build/FilePathProvider.o

#STEER BY WIRE
SBW_OBJ_FILES =	build/SteerByWire.o 			\
				build/Ipc.o						\
				build/Utils.o					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/FilePathProvider.o
#THROTTLE CONTROL
TC_OBJ_FILES = 	build/ThrottleControl.o 		\
				build/Ipc.o						\
				build/Utils.o					\
				build/Logger.o 					\
				build/Random.o 					\
				build/DateProvider.o			\
				build/FilePathProvider.o

#BRAKE BY WIRE
BBW_OBJ_FILES = build/BrakeByWire.o 			\
				build/Ipc.o						\
				build/Utils.o					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/FilePathProvider.o

#FRONT WIND SHIELD CAMERA
FWC_OBJ_FILES = build/FrontWindshieldCamera.o	\
				build/Ipc.o						\
				build/Utils.o					\
 				build/Logger.o 					\
 				build/DateProvider.o			\
				build/CentralEcuIpc.o			\
				build/FilePathProvider.o

#FORWARD FACING RADAR
FFR_OBJ_FILES = build/ForwardFacingRadar.o 		\
				build/Ipc.o						\
				build/Utils.o					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/CentralEcuIpc.o			\
				build/FilePathProvider.o

#PARK ASSIST
PA_OBJ_FILES = build/ParkAssist.o 				\
				build/Ipc.o						\
				build/Utils.o					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/CentralEcuIpc.o			\
				build/FilePathProvider.o

#SURROUND VIEW CAMERAS
SVC_OBJ_FILES = build/SurroundViewCameras.o 	\
				build/Ipc.o						\
				build/Utils.o					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/FilePathProvider.o

#CENTRAL ECU
ECU_OBJ_FILES = build/CentralEcu.o 				\
				build/Ipc.o 					\
				build/Utils.o					\
				build/Logger.o 					\
				build/DateProvider.o			\
				build/FilePathProvider.o		\
				build/CentralEcuHmiIpc.o		\
				build/CentralEcuBbwIpc.o		\
				build/CentralEcuSbwIpc.o		\
				build/CentralEcuTcIpc.o			\
				build/CentralEcuPaIpc.o

#			END OBJ FILE DEPENDENCIES			#

DEBUG ?= 0

ifeq ($(DEBUG), 1)
    CFLAGS = -g
else
    CFLAGS =
endif

all: buildFileObjects buildApp

buildApp: initializeAppDirectory $(HMI_OBJ_FILES) $(SBW_OBJ_FILES) $(TC_OBJ_FILES) $(BBW_OBJ_FILES) $(FWC_OBJ_FILES) $(FFR_OBJ_FILES) $(PA_OBJ_FILES) $(SVC_OBJ_FILES) $(ECU_OBJ_FILES)
	$(CC) $(HMI_OBJ_FILES)  -o app/hmi $(CFLAGS)
	$(CC) $(SBW_OBJ_FILES)  -o app/sbw $(CFLAGS)
	$(CC) $(TC_OBJ_FILES)  -o app/tc $(CFLAGS)
	$(CC) $(BBW_OBJ_FILES)  -o app/bbw $(CFLAGS)
	$(CC) $(FWC_OBJ_FILES)  -o app/fwc $(CFLAGS)
	$(CC) $(FFR_OBJ_FILES)  -o app/ffr $(CFLAGS)
	$(CC) $(PA_OBJ_FILES)  -o app/pa $(CFLAGS)
	$(CC) $(SVC_OBJ_FILES)  -o app/svc $(CFLAGS)
	$(CC) $(ECU_OBJ_FILES)  -o app/ecu $(CFLAGS)

initializeAppDirectory: $(NEED_FILES_IN_APP_FROM_SRC)
	$(AT) mkdir -p app
	$(foreach file, $^, cp $(file) $(addprefix app/,$(notdir $(file)));)

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