SRC_DIR = src
I2C_SRC_DIR = ../../../libraries/i2c/src
BME280_SRC_DIR = ../../../libraries/bme280/src
INC_DIR = include
BUILD_DIR = build
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

TARGET = atmosphere_slave
SRC_FILES = $(SRC_DIR)/atmosphere_slave.c \
			$(I2C_SRC_DIR)/i2c.c \
			$(BME280_SRC_DIR)/bme.c
I2C_MODE = -DI2C_SLAVE

# default for testing purposes
MCU ?= attiny1614

ATTINY_INC = ../../../libraries/attiny/include
I2C_INC = ../../../libraries/i2c/include
BME280_INC = ../../../libraries/bme280/include

ifeq ($(MCU), attiny1614)
MMCU		= attiny1614
F_CPU		= 20000000UL
PROGRAMMER	= serialupdi
PORT		= /dev/ttyUSB0
BAUD		= 115200
AVRDUDE_MCU	= attiny1614
CFLAGS		+= -B ../../../libraries/attiny/gcc/dev/attiny1614

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
CFLAGS		+= -B ../../../libraries/attiny/gcc/dev/attiny412

else
$(error Unsupported MCU: $(MCU). use attiny1614, attiny412 or atmega328p)
endif

# can change this later if i2c is too slow
I2C_FREQ	= 100000UL

# Set base CFLAGS now that MMCU is defined
CFLAGS += -mmcu=$(MMCU)

# Add include paths globally
CFLAGS += -I$(INC_DIR) -I$(I2C_INC) -I$(BME280_INC) -I$(ATTINY_INC)

# FLAGS
CFLAGS += -DF_CPU=$(F_CPU)
CFLAGS += -DI2C_FREQ=$(I2C_FREQ)
CFLAGS += $(I2C_MODE)
CFLAGS += -Os
CFLAGS += -Wall -Werror -Wextra
CFLAGS += -std=c11

LDFLAGS = -mmcu=$(MMCU)

OBJ_FILES	= $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRC_FILES)))
ELF			= $(BUILD_DIR)/$(TARGET).elf
HEX			= $(BUILD_DIR)/$(TARGET).hex

# TARGETS

all: $(HEX)

$(ELF): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.ino | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(I2C_SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(BME280_SRC_DIR)/%.c | $(BUILD_DIR)
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
