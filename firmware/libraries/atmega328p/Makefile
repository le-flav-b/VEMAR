SHELL		?=	/bin/zsh
SYSTEM		:=	$(shell uname -s)

NAME		=	libvemar.a
FREQUENCY	?=	16000000UL
BAUDRATE	?=	115200
PORT		?=	/dev/ttyUSB0
PROGRAMMER	?=	arduino
MCU			?=	atmega328p

ifeq ($(SYSTEM), Darwin)
PORT		:=	/dev/tty.usbserial-1410
endif

CC			:=	avr-gcc
AR			:=	avr-ar rcs
OBJCOPY		:=	avr-objcopy
AVRDUDE		:=	avrdude
RM			:=	rm -f
ECHO		:=	echo
MKDIR		:=	mkdir -p

INCLUDE		=	include

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
				-DBAUDRATE=$(BAUDRATE) \
				-I$(INCLUDE) \
				-DI2C_MASTER \
				-DI2C_FREQ=100000UL \
				-DSLAVE_ADDR=0x10

AVRFLAGS	=	-p $(MCU) \
				-c $(PROGRAMMER) \
				-b $(BAUDRATE) \
				-P $(PORT) \
				-v \
				-D

SOURCES		=	main.c \
				gpio.c \
				adc.c \
				uart.c \
				spi.c \
				serial.c \
				joystick.c \
				nrf24l01.c \
				radio.c \
				ili9341.c \
				tft.c \
				util.c \
				timer.c \
				pwm.c \
				i2c.c

BUILD_DIR	=	build
SOURCE_DIR	=	src

OBJECTS		=	$(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))

FILE_HEX	=	$(addsuffix .hex, $(NAME))
FILE_BIN	=	$(addsuffix .bin, $(NAME))

COLOR_LOG	=	"\033[96m\033[1m"
COLOR_RESET	=	"\033[0m"

.PHONY: all

all: $(NAME)

$(NAME): $(OBJECTS)
	@$(ECHO) $(COLOR_LOG) "Generating static library: '$@'" $(COLOR_RESET)
	$(AR) $@ $^

.PHONY: clean fclean rebuild

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) -r $(BUILD_DIR)
	$(RM) $(NAME)

rebuild: fclean all

.PHONY: help

help:
	@$(ECHO) "Syntax: make [PARAM=VALUE] [...]"
	@$(ECHO) "- FREQUENCY"
	@$(ECHO) "- BAUDRATE"
	@$(ECHO) "- PORT"
	@$(ECHO) "- PROGRAMMER"

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	@$(ECHO) $(COLOR_LOG) "Building OBJ file: '$@'" $(COLOR_RESET)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR):
	$(MKDIR) $@
