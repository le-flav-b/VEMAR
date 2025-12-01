#########################
######  STRUCTURE  ######
#########################

SRC_DIR			=	src
INC_DIRS		=	include		include/lib		include/module
BUILD_DIR		=	build
OBJ_DIR			=	${BUILD_DIR}/obj
DEP_DIR			=	${BUILD_DIR}/dep

BIN_FILE		=	${BUILD_DIR}/prog.bin
HEX_FILE		=	${BUILD_DIR}/prog.hex


#######################
######  SOURCES  ######
#######################

MAIN_SRCS_DIR	=	${SRC_DIR}
MAIN_SRCS		=	${wildcard ${MAIN_SRCS_DIR}/*.c}

LIB_SRCS_DIR	=	${SRC_DIR}/lib
LIB_SRCS		=	${wildcard ${LIB_SRCS_DIR}/*.c}

MODULE_SRCS_DIR	=	${SRC_DIR}/module
MODULE_SRCS		=	${wildcard ${MODULE_SRCS_DIR}/*.c}


##########################
######  MCU CONFIG  ######
##########################

MCU				=	atmega328p
ID_MCU			=	m328p
UART_BAUDRATE	=	115200
F_CPU			=	16000000UL
PORT			=	/dev/ttyUSB0


###########################
######  COMPILATION  ######
###########################

SRCS			=	${MAIN_SRCS}	${LIB_SRCS}		${MODULE_SRCS}
OBJS			=	${SRCS:${SRC_DIR}/%.c=${OBJ_DIR}/%.o}
DEPS			=	${SRCS:${SRC_DIR}/%.c=${DEP_DIR}/%.d}

GET_DEP_PATH	=	${@:${OBJ_DIR}/%.o=${DEP_DIR}/%.d}

CC				=	avr-gcc
CFLAGS			=	-O \
					-mmcu=${MCU} \
					-DF_CPU=${F_CPU} \
					-DUART_BAUDRATE=${UART_BAUDRATE} \
					${addprefix -I,${INC_DIRS}}

CBUILD			=	-c \
					-MMD -MF ${GET_DEP_PATH}

MKDIR			=	@mkdir -vp
RM				=	@rm -vrf


#####################
######  RULES  ######
#####################

all: hex flash

-include ${DEPS}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	${MKDIR} ${dir $@} ${dir ${GET_DEP_PATH}}
	${CC} ${CFLAGS} ${CBUILD} $< -o $@

${BIN_FILE}: ${OBJS}
	${CC} ${CFLAGS} $^ -o $@

${HEX_FILE}: ${BIN_FILE}
	avr-objcopy -O ihex $< $@

hex: ${HEX_FILE}

flash:
	avrdude -U ${HEX_FILE} -P ${PORT} -p ${ID_MCU} -c arduino -b ${UART_BAUDRATE}

clean:
	${RM} ${BUILD_DIR}

re: clean all

.PHONY: all hex flash clean re
