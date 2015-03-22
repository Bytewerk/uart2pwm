# --- START OF CONFIG ---------------------------------------------------
# Edit the following variables for your own needs

# toolchain configuration
PREFIX  ?= avr
CC       = $(PREFIX)-gcc
LD       = $(PREFIX)-gcc
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
SIZE     = $(PREFIX)-size
GDB      = $(PREFIX)-gdb
AVRDUDE  = avrdude

TOOLCHAIN_DIR ?= /usr/avr

MCU := attiny2313

# avrdude config
AVRDUDE_PORT = usb
AVRDUDE_PROG = usbtiny
AVRDUDE_PART = $(MCU)

# default build configuration
# "make BUILD=release" does a release build
BUILD:=release

# basic build flags configuration
CFLAGS+=-mmcu=$(MCU) -Wall -pedantic -std=gnu99
LDFLAGS+=-mmcu=$(MCU)

# hack for missing header files:
# http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/index_FDS.html
CFLAGS += -I$(HOME)/Software/msp430/msp430-gcc-support-files
LDFLAGS += -L$(HOME)/Software/msp430/msp430-gcc-support-files

# build type specific flags
CFLAGS_debug=-O0 -ggdb -DDEBUG
LDFLAGS_debug=

CFLAGS_release=-O2
LDFLAGS_release=

# target configuration
TARGET := nas_rgb_pwm
VERSION := 0.0.0
VCSVERSION := $(shell git rev-parse --short HEAD)

# source files for the project
SOURCE := $(shell find src/ -name '*.c')
INCLUDES := $(shell find src/ -name '*.h')

# additional dependencies for build (proper targets must be specified by user)
DEPS :=

# default target
all: $(TARGET)

# user-specific targets

# --- END OF CONFIG -----------------------------------------------------

OBJ1=$(patsubst %.c, %.o, $(SOURCE))
OBJ=$(patsubst src/%, obj/$(BUILD)/%, $(OBJ1))

VERSIONSTR="\"$(VERSION)-$(VCSVERSION)\""

CFLAGS+=-DVERSION=$(VERSIONSTR)

TARGET_BASE := bin/$(BUILD)/$(TARGET)

CFLAGS+=$(CFLAGS_$(BUILD))
LDFLAGS+=$(LDFLAGS_$(BUILD))

.PHONY show_cflags:
	@echo --- Build parameters:  ------------------------------------------
	@echo CFLAGS\=$(CFLAGS)
	@echo LDFLAGS\=$(LDFLAGS)
	@echo SOURCE\=$(SOURCE)
	@echo -----------------------------------------------------------------

$(TARGET): show_cflags $(TARGET_BASE).elf $(TARGET_BASE).hex \
	         $(TARGET_BASE).lss $(TARGET_BASE).bin
	@echo -e "\nSize:"
	@$(SIZE) $(TARGET_BASE).elf
	@echo
	@echo ">>> $(BUILD) build complete."

$(TARGET_BASE).elf: $(DEPS) $(OBJ) $(INCLUDES) Makefile
	@echo Linking $@ ...
	@mkdir -p $(shell dirname $@)
	@$(LD) -o $(TARGET_BASE).elf $(OBJ) $(LDFLAGS)

$(TARGET_BASE).hex: $(TARGET_BASE).elf
	@echo "Generating $@ ..."
	@$(OBJCOPY) -Oihex $< $@

$(TARGET_BASE).bin: $(TARGET_BASE).elf
	@echo "Generating $@ ..."
	@$(OBJCOPY) -Obinary $< $@

$(TARGET_BASE).lss: $(TARGET_BASE).elf
	@echo "Generating $@ ..."
	@$(OBJDUMP) -S $< > $@

obj/$(BUILD)/%.o: src/%.c $(INCLUDES) Makefile
	@echo "Compiling $< ..."
	@mkdir -p $(shell dirname $@)
	$(CC) -c $(CFLAGS) -o $@ $<

program: $(TARGET)
	$(AVRDUDE) -P $(AVRDUDE_PORT) -p $(AVRDUDE_PART) -c $(AVRDUDE_PROG) -U flash:w:$(TARGET_BASE).hex

debug: $(TARGET)
	echo "Not supported"

clean:
	rm -f $(TARGET_BASE).elf
	rm -f $(TARGET_BASE).hex
	rm -f $(TARGET_BASE).lss
	rm -f $(TARGET_BASE).bin
	rm -f $(OBJ)
