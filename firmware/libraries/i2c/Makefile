TARGET = i2c
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

SRC_FILES = $(SRC_DIR)/atmosphere_slave.c \
			$(SRC_DIR)/i2c.c

CFLAGS = -mmcu=$(MMCU)

# default for testing purposes
MCU ?= attiny1614

ATTINY_INC = ../attiny/include

ifeq ($(MCU), attiny1614)
MMCU		= attiny1614
F_CPU		= 16000000UL
PROGRAMMER	= serialupdi
PORT		= /dev/ttyUSB0
BAUD		= 115200
AVRDUDE_MCU	= attiny1614
CFLAGS		+= -I$(ATTINY_INC) -B ../attiny/gcc/dev/attiny1614

else ifeq ($(MCU), atmega328p)
MMCU		= atmega328p
# change to 8000000UL if running at 8MHZ (3V3)
F_CPU		= 16000000UL
# unsure for PROGRAMMER
PROGRAMMER	= usbtiny
# change if running on windows/mac
PORT		= /dev/ttyUSB0
BAUD		= 19200
AVRDUDE_MCU	= m328p

else ifeq ($(MCU), attiny412)
MMCU		= attiny412
F_CPU		= 16000000UL
PROGRAMMER	= serialupdi
PORT		= /dev/ttyUSB0
BAUD		= 115200
AVRDUDE_MCU	= attiny412
CFLAGS		+= -I$(ATTINY_INC) -B ../attiny/gcc/dev/attiny412

else
$(error Unsupported MCU: $(MCU). use attiny1614, attiny412 or atmega328p)
endif

# can change this later if i2c is too slow
I2C_FREQ	= 100000UL

# FLAGS


CFLAGS += -DF_CPU=$(F_CPU)
CFLAGS += -DI2C_FREQ=$(I2C_FREQ)
CFLAGS += -I$(INC_DIR)
# CFLAGS += $(addprefix -I, $(shell find $(INC_DIR) -type d))
CFLAGS += -Os
CFLAGS += -Wall -Werror -Wextra
CFLAGS += -std=c11

# CFLAGS += -DI2C_MASTER
CFLAGS += -DI2C_SLAVE

LDFLAGS = -mmcu=$(MMCU)

OBJ_FILES	= $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))
ELF			= $(BUILD_DIR)/$(TARGET).elf
HEX			= $(BUILD_DIR)/$(TARGET).hex

# TARGETS

all: $(HEX)

$(ELF): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

flash: $(HEX)
	$(AVRDUDE) -c $(PROGRAMMER) -p $(AVRDUDE_MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(HEX):i

clean:
	rm -rf $(BUILD_DIR)
