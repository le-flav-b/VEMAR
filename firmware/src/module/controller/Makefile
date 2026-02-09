SHELL		:=	/bin/zsh
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

INCLUDE		=	../../../include/module/controller

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
				serial.c \
				joystick.c

BUILD_DIR	=	build

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

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@$(ECHO) $(COLOR_LOG) "Building OBJ file: '$@'" $(COLOR_RESET)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR):
	$(MKDIR) $@
