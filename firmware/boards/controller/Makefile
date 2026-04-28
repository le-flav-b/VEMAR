SHELL		:= /bin/zsh
SYSTEM		:= $(shell uname -s)

NAME		=	controller

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
MV			:=	mv

INC_ATMEGA	=	../../libraries/atmega328p/include
INC_PACKET	=	../../libraries


LIBRARY_DIR	=	../../libraries/atmega328p
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
				-I$(INC_ATMEGA) \
				-I$(INC_PACKET)

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

ifeq ($(MAKECMDGOALS), debug)
CFLAGS		+=	-DVEMAR_DEBUG_ENABLED
endif

all: $(NAME)

debug: fclean all

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
	$(RM) $(LIBRARY)

%.bin: $(BUILD_DIR)/%.o $(LIBRARY)
	@$(ECHO) $(COLOR_LOG) "Building BIN file: '$@'" $(COLOR_RESET)
	$(CC) $(LDFLAGS) -o $@ $< -L. -lvemar

%.hex: %.bin
	@$(ECHO) $(COLOR_LOG) "Building HEX file: '$@'" $(COLOR_RESET)
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@$(ECHO) $(COLOR_LOG) "Building OBJ file: '$@'" $(COLOR_RESET)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR):
	@$(ECHO) $(COLOR_LOG) "Creating Build Directory: '$@'" $(COLOR_RESET)
	mkdir -p $@

$(LIBRARY):
	@$(ECHO) $(COLOR_LOG) "Building Library" $(COLOR_RESET)
	$(MAKE) $(LIBRARY_DIR)
	@$(MV) $(LIBRARY_DIR)/$(LIBRARY) .

.PHONY: screen

screen:
	$(SCREEN) $(PORT) $(BAUDRATE)

.PHONY: lib

lib:
	@$(ECHO) $(COLOR_LOG) "Rebuilding library" $(COLOR_RESET)
	@$(MAKE) $(LIBRARY_DIR) rebuild
	@$(MV) $(LIBRARY_DIR)/$(LIBRARY) .

.PRECIOUS: $(BUILD_DIR)/%.o %.bin

controller: controller.hex
