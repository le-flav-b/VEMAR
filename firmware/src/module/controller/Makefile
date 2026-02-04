SYSTEM		= $(shell uname -s)

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
SCREEN		:=	screen

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

# LDFLAGS		=	-Os \
# 				-mmcu=$(MCU) \
# 				-ffunction-sections \
# 				-fdata-sections \
# 				-Wl,--gc-sections

AVRFLAGS	=	-p $(MCU) \
				-c $(PROGRAMMER) \
				-b $(BAUDRATE) \
				-P $(PORT) \
				-v \
				-D

SOURCES		=	main.c \
				adc.c \
				joystick.c \
				uart.c \
				spi.c \
				print.c \
				pin.c \
				button.c \
				led.c

BUILD_DIR	=	build

OBJECTS		=	$(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))

FILE_HEX	=	$(addsuffix .hex, $(NAME))
FILE_BIN	=	$(addsuffix .bin, $(NAME))

all: $(NAME)

$(NAME): $(OBJECTS)
	$(AR) $@ $^

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) -r $(BUILD_DIR)
	$(RM) $(NAME)

rebuild: fclean all

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR):
	mkdir -p $@

.PHONY: all hex flash clean fclean rebuild
