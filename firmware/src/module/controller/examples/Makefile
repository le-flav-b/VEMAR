SHELL		:= /bin/zsh
SYSTEM		:= $(shell uname -s)

NAME		=	blink \
				button \
				toggle \
				serial \
				analog \
				joystick \
				spi \
				transmitter \
				receiver \
				transmitter2 \
				receiver2

FREQUENCY	?=	16000000UL
BAUDRATE	?=	115200
PORT		?=	/dev/ttyUSB0
PROGRAMMER	?=	arduino
MCU			?=	atmega328p

ifeq ($(SYSTEM), Darwin)
PORT		:=	/dev/tty.usbserial-1410
endif

CC			:=	avr-gcc
OBJCOPY		:=	avr-objcopy
AVRDUDE		:=	avrdude
RM			:=	rm -f
SCREEN		:=	screen
MAKE		:=	make -C
ECHO		:=	echo
MKDIR		:=	mkdir -p

INCLUDE		=	../../../../include/module/controller

LIBRARY_DIR	=	..
LIBRARY		=	libvemar.a

CFLAGS		=	-Wall \
				-Wextra \
				-Werror \
				-pedantic \
				-std=gnu99 \
				-Os \
				-ffunction-sections \
				-fdata-sections \
				-mmcu=$(MCU) \
				-DF_CPU=$(FREQUENCY) \
				-I$(INCLUDE)

LDFLAGS		=	-Os \
				-mmcu=$(MCU) \
				-ffunction-sections \
				-fdata-sections \
				-Wl,--gc-sections

AVRFLAGS	=	-p $(MCU) \
				-c $(PROGRAMMER) \
				-b $(BAUDRATE) \
				-P $(PORT) \
				-v \
				-D

SOURCES		=	$(addsuffix .c, $(NAME))

BUILD_DIR	=	build

OBJECTS		=	$(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))

FILE_HEX	=	$(addsuffix .hex, $(NAME))
FILE_BIN	=	$(addsuffix .bin, $(NAME))

COLOR_LOG	=	"\033[96m\033[1m"
COLOR_RESET	=	"\033[0m"

EXAMPLES	=	$(foreach example, $(NAME), "  - $(example)\n")

.PHONY: help

help:
	@$(ECHO) "Select an example:"
	@$(ECHO) "" $(EXAMPLES)

.PHONY: $(NAME)

$(NAME):
	$(AVRDUDE) $(AVRFLAGS) -U flash:w:$<:i

.PHONY: clean fclean

clean:
	@$(ECHO) $(COLOR_LOG) "Removing OBJ files" $(COLOR_RESET)
	$(RM) $(OBJECTS)

fclean: clean
	@$(ECHO) $(COLOR_LOG) "Removing Building Directory" $(COLOR_RESET)
	$(RM) -r $(BUILD_DIR)
	$(RM) $(FILE_BIN) $(FILE_HEX)

%.bin: $(BUILD_DIR)/%.o $(LIBRARY_DIR)/$(LIBRARY)
	@$(ECHO) $(COLOR_LOG) "Building BIN file: '$@'" $(COLOR_RESET)
	$(CC) $(LDFLAGS) -o $@ $< -L$(LIBRARY_DIR) -lvemar

%.hex: %.bin
	@$(ECHO) $(COLOR_LOG) "Building HEX file: '$@'" $(COLOR_RESET)
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@$(ECHO) $(COLOR_LOG) "Building OBJ file: '$@'" $(COLOR_RESET)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR):
	@$(ECHO) $(COLOR_LOG) "Creating Build Directory: '$@'" $(COLOR_RESET)
	mkdir -p $@

$(LIBRARY_DIR)/$(LIBRARY):
	@$(ECHO) $(COLOR_LOG) "Building Library" $(COLOR_RESET)
	$(MAKE) $(LIBRARY_DIR)

.PHONY: screen

screen:
	$(SCREEN) $(PORT) $(BAUDRATE)

.PHONY: lib

lib:
	@$(ECHO) $(COLOR_LOG) "Rebuilding library" $(COLOR_RESET)
	$(MAKE) $(LIBRARY_DIR) rebuild

.PRECIOUS: $(BUILD_DIR)/%.o %.bin

blink: blink.hex
analog: analog.hex
button: button.hex
toggle: toggle.hex
serial: serial.hex
joystick: joystick.hex
spi: spi.hex
transmitter: transmitter.hex
receiver: receiver.hex
transmitter2: transmitter2.hex
receiver2: receiver2.hex
