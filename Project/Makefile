CC = gcc
CFLAGS =
C_FILES = $(wildcard src/*.c)
HMI_OBJ_FILES = build/HumanMachineInterface.o
SBW_OBJ_FILES = build/SteerByWire.o build/Logger.o
TC_OBJ_FILES = build/ThrottleControl.o build/Logger.o build/Random.o
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

buildFileObjects: $(C_FILES)
	$(AT) mkdir -p build
	$(foreach file, $^, ${CC} -c $(file) -o $(file:src/%.c=build/%.o) $(CFLAGS);)

clean:
	@echo Removing application
	$(AT)-rm -rf app
	@echo Removing build
	$(AT)-rm -rf build
