NAME		=	joy
FREQUENCY	?=	16000000
BAUDRATE	?=	115200
PORT		?=	/dev/ttyUSB0
PROGRAMMER	?=	arduino
MCU			?=	atmega328p

CC			=	avr-gcc
OBJCOPY		=	avr-objcopy
AVRDUDE		=	avrdude
RM			?=	rm -f

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

LDFLAGS		=	-Os \
				-mmcu=$(MCU) \
				-ffunction-sections \
				-fdata-sections \
				-Wl,--gc-sections

AVRFLAGS	=	-p $(MCU) \
				-c $(PROGRAMMER) \
				-b $(BAUDRATE) \
				-P $(PORT) 

SOURCES		=	main.c \
				adc.c \
				joystick.c

BUILD_DIR	=	build

OBJECTS		=	$(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))

FILE_HEX	=	$(addsuffix .hex, $(NAME))
FILE_BIN	=	$(addsuffix .bin, $(NAME))

all: hex flash

hex: $(FILE_BIN)

flash: $(FILE_HEX)
	$(AVRDUDE) $(AVRFLAGS) -D -U flash:w:$<

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) -r $(BUILD_DIR)
	$(RM) $(FILE_BIN) $(FILE_HEX)

re: fclean all

$(FILE_BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(FILE_HEX): $(FILE_BIN)
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR):
	mkdir -p $@

.PHONY: all hex flash clean fclean re
