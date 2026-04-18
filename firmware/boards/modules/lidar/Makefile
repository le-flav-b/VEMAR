#########################
######  STRUCTURE  ######
#########################

NAME			=	lidar_firmware
SRC_DIR			=	src
INC_DIRS		=	include
BUILD_DIR		=	build
OBJ_DIR			=	${BUILD_DIR}/obj
DEP_DIR			=	${BUILD_DIR}/dep

BIN_FILE		=	${BUILD_DIR}/${NAME}.bin
HEX_FILE		=	${BUILD_DIR}/${NAME}.hex


#######################
######  SOURCES  ######
#######################

MAIN_SRCS_DIR	=	${SRC_DIR}
MAIN_SRCS		=	${wildcard ${MAIN_SRCS_DIR}/*.c}

SRCS			=	${MAIN_SRCS}


##########################
######  MCU CONFIG  ######
##########################

MCU				=	atmega128a
F_CPU			=	16000000UL


###########################
######  COMPILATION  ######
###########################

OBJS			=	${SRCS:${SRC_DIR}/%.c=${OBJ_DIR}/%.o}
DEPS			=	${SRCS:${SRC_DIR}/%.c=${DEP_DIR}/%.d}

GET_DEP_PATH	=	${@:${OBJ_DIR}/%.o=${DEP_DIR}/%.d}

CC				=	avr-gcc

CC_BUILD_FLAGS	=	-mmcu=${MCU} \
					-c \
					-MMD -MF ${GET_DEP_PATH} \
					-DF_CPU=${F_CPU} \
					${addprefix -I,${INC_DIRS}} \
					-Wall \
					-Wextra \
					-O3 \
					-ffunction-sections \
					-fdata-sections

CC_LINK_FLAGS	=	-mmcu=${MCU} \
					-Wl,--gc-sections


######################
######  UPLOAD  ######
######################

PORT			=	/dev/ttyUSB0
PROGRAMMER		=	stk500v1
UART_BAUDRATE	=	19200

OBJCOPY			=	avr-objcopy
OBJCOPY_FLAGS	=	-O ihex

AVRDUDE			=	avrdude
AVRDUDE_FLAGS	=	-U ${HEX_FILE} \
					-P ${PORT} \
					-p ${MCU} \
					-c ${PROGRAMMER} \
					-b ${UART_BAUDRATE}


#####################
######  RULES  ######
#####################

all: hex flash

-include ${DEPS}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	@mkdir -vp ${dir $@} ${dir ${GET_DEP_PATH}}
	${CC} ${CC_BUILD_FLAGS} $< -o $@

${BIN_FILE}: ${OBJS}
	${CC} ${CC_LINK_FLAGS} $^ -o $@

${HEX_FILE}: ${BIN_FILE}
	${OBJCOPY} ${OBJCOPY_FLAGS} $< $@

hex: ${HEX_FILE}

flash:
	${AVRDUDE} ${AVRDUDE_FLAGS}

clean:
	@rm -vrf ${BUILD_DIR}

re: clean all

.PHONY: all hex flash clean re
