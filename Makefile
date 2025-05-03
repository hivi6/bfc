PLATFORM := $(shell uname -m)
C_FILES := $(shell find src -maxdepth 1 -name '*.c')

TARGET_FILE := src/target/amd64.c
ifeq ($(PLATFORM), arm64)
TARGET_FILE := src/target/arm64.c
endif

all: ${C_FILES} ${TARGET_FILE}
	gcc -o bfc $(C_FILES) $(TARGET_FILE)

.PHONY: clean
clean:
	rm bfc
