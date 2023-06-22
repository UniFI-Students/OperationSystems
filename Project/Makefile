CC = gcc
CFLAGS =
C_FILES = $(shell find src -name '*.c')

HMI_OBJ_FILES = build/HumanMachineInterface.o build/Ipc.o build/Logger.o build/DateProvider.o
SBW_OBJ_FILES = build/SteerByWire.o build/Logger.o build/DateProvider.o
TC_OBJ_FILES = build/ThrottleControl.o build/Logger.o build/Random.o build/DateProvider.o
BBW_OBJ_FILES = build/BreakByWire.o build/Logger.o build/DateProvider.o
FWSC_OBJ_FILES = build/FrontWindshieldCamera.o build/Logger.o build/DateProvider.o
FFR_OBJ_FILES = build/ForwardFacingRadar.o build/Logger.o build/DateProvider.o
PA_OBJ_FILES = build/ParkAssist.o build/Logger.o build/DateProvider.o
SVC_OBJ_FILES = build/SurroundViewCameras.o build/Logger.o build/DateProvider.o
CECU_OBJ_FILES = build/CentralEcu.o build/Logger.o build/Ipc.o build/DateProvider.o

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
	$(CC) $(FFR_OBJ_FILES)  -o app/ffr $(CFLAGS)
	$(CC) $(PA_OBJ_FILES)  -o app/pa $(CFLAGS)
	$(CC) $(SVC_OBJ_FILES)  -o app/svc $(CFLAGS)
	$(CC) $(CECU_OBJ_FILES)  -o app/cecu $(CFLAGS)

buildFileObjects: $(C_FILES)
	$(AT) mkdir -p build
	$(foreach file, $^, ${CC} -c $(file) -o $(addprefix build/,$(notdir $(file:%.c=%.o))) $(CFLAGS);)

clean:
	@echo Removing application
	$(AT)-rm -rf app
	@echo Removing build
	$(AT)-rm -rf build
