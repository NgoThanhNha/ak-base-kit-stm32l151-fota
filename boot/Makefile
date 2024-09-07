# Include sources file
include app/Makefile.mk
include drivers/Makefile.mk
include sys/Makefile.mk
include common/Makefile.mk

# Utilitis define
Print = @echo "~"
print = @echo

# Name of build PROJECT ex: ak-base-kit-stm32l151-fota-boot.bin
NAME_MODULE = ak-base-kit-stm32l151-fota-boot
PROJECT = $(NAME_MODULE)
OBJECTS_DIR = build
TARGET = $(OBJECTS_DIR)/$(NAME_MODULE).axf

OBJECTS = $(addprefix $(OBJECTS_DIR)/,$(notdir $(SOURCES_ASM:.s=.o)))
OBJECTS += $(addprefix $(OBJECTS_DIR)/,$(notdir $(SOURCES:.c=.o)))
OBJECTS += $(addprefix $(OBJECTS_DIR)/,$(notdir $(SOURCES_CPP:.cpp=.o)))

GCC_PATH		= $(HOME)/workspace/tools/gcc-arm-none-eabi-10.3-2021.10
PROGRAMER_PATH		= $(HOME)/workspace/tools/STM32CubeProgrammer/bin

# App start address, that need sync with declare in linker file and interrupt vector table.
BOOT_START_ADDR = 0x08000000

OPTIMIZE_OPTION = -g -Os

LIBC		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libc.a
LIBM		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libm.a
LIBFPU		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libg.a
LIBRDPMON	= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/librdpmon.a
LIBSTDCPP_NANO	= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libstdc++_nano.a

LIBGCC		= $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp/libgcc.a
LIBGCOV		= $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp/libgcov.a

LIB_PATH += -L$(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp
LIB_PATH += -L$(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp

# The command for calling the compiler.
CC		=	$(GCC_PATH)/bin/arm-none-eabi-gcc
CPP		=	$(GCC_PATH)/bin/arm-none-eabi-g++
AR		=	$(GCC_PATH)/bin/arm-none-eabi-ar
AS		=	$(GCC_PATH)/bin/arm-none-eabi-gcc -x assembler-with-cpp
LD 		= 	$(GCC_PATH)/bin/arm-none-eabi-ld
OBJCOPY		=	$(GCC_PATH)/bin/arm-none-eabi-objcopy
OBJNM		=	$(GCC_PATH)/bin/arm-none-eabi-nm
ARM_SIZE	=	$(GCC_PATH)/bin/arm-none-eabi-size

# Set the compiler CPU/FPU options.
CPU = -mthumb -march=armv7-m
FPU = -mfloat-abi=soft

GENERAL_FLAGS +=			\
		$(OPTIMIZE_OPTION)	\
		-DNDEBUG		\
		-DUSE_STDPERIPH_DRIVER	\
		-DSTM32L1XX_MD		\

# C compiler flags
CFLAGS +=	\
		$(CPU)			\
		$(FPU)			\
		-ffunction-sections	\
		-fdata-sections		\
		-fstack-usage		\
		-MD			\
		-Wall			\
		-Wno-enum-conversion	\
		-std=c99		\
		-c			\
		$(GENERAL_FLAGS)	\

# C++ compiler flags
CPPFLAGS += $(CPU)			\
		$(FPU)			\
		-ffunction-sections	\
		-fdata-sections		\
		-fstack-usage		\
		-fno-rtti		\
		-fno-exceptions		\
		-fno-use-cxa-atexit	\
		-MD			\
		-Wall			\
		-std=c++11		\
		-c			\
		$(GENERAL_FLAGS)	\

# Linker file
LDFILE = sys/stk.ld

# Linker flags
LDFLAGS	=	-Map=$(OBJECTS_DIR)/$(PROJECT).map	\
		--gc-sections	\
		$(LIB_PATH)	\
		$(LIBC) $(LIBM) $(LIBSTDCPP_NANO) $(LIBGCC) $(LIBGCOV) $(LIBFPU) $(LIBRDPMON)

all: build $(TARGET)

build:
	$(Print) CREATE $(OBJECTS_DIR) folder
	@mkdir -p $(OBJECTS_DIR)

$(TARGET): $(OBJECTS) $(LIBC) $(LIBM) $(LIBSTDCPP_NANO) $(LIBGCC) $(LIBGCOV) $(LIBFPU) $(LIBRDPMON)
	$(Print) LD $@
	@$(LD) --entry reset_handler -T $(LDFILE) $(LDFLAGS) -o $(@) $(^)
	$(Print) OBJCOPY $(@:.axf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.out)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.elf)
	@$(ARM_SIZE) $(TARGET)

$(OBJECTS_DIR)/%.o: %.c
	$(Print) CC $@
	@$(CC) $(CFLAGS) -o $@ $<

$(OBJECTS_DIR)/%.o: %.cpp
	$(Print) CXX $@
	@$(CPP) $(CPPFLAGS) -o $@ $<

$(OBJECTS_DIR)/%.o: %.s
	$(Print) AS $@
	@$(AS) $(CFLAGS) -o $@ $<

help:
	$(print) ""
	$(print) " ___  ____  _  _ "
	$(print) "/ __)(_  _)( )/ )"
	$(print) "\\__ \\  )(  (   ("
	$(print) "(___/ (__) (_)\\_)"
	$(print) ""

	$(print) "How to use?"
	$(print) ""

	$(print) "[make build] complile the code"
	$(print) "[make flash] burn firmware via st-link"
	$(print) "[make clean] clean build project folder"
	$(print) "[make sym] create list symbol from objects file"
	$(print) "[make view_sym] view list symbol size"
	$(print) ""

# # For Linux
# flash: all
# 	$(PROGRAMER_PATH)/STM32_Programmer.sh -c port=SWD -w $(TARGET:.axf=.bin) $(BOOT_START_ADDR) -rst

# clean:
# 	$(Print) CLEAN $(OBJECTS_DIR) folder
# 	@rm -rf $(OBJECTS_DIR)

# For Windows
clean:
	$(Print) CLEAN $(OBJECTS_DIR) folder
	@if exist build (rmdir /s /q $(OBJECTS_DIR) -p)

flash: all
	st-link_cli.exe -c SWD -P $(TARGET:.axf=.bin) $(BOOT_START_ADDR) -Rst

sym: $(TARGET)
	$(Print) export object name $(<:.axf=.sym)
	$(OBJNM) --size-sort --print-size $(<) >> $(<:.axf=.sym)

view_sym:
	cat $(OBJECTS_DIR)/$(NAME_MODULE).sym