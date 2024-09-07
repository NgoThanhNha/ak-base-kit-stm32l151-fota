CFLAGS += -I./sys/peripheral_driver/CMSIS/Device/ST/STM32L1xx/Include
CFLAGS += -I./sys/peripheral_driver/CMSIS/Include
CPPFLAGS += -I./sys/peripheral_driver/CMSIS/Device/ST/STM32L1xx/Include
CPPFLAGS += -I./sys/peripheral_driver/CMSIS/Include

VPATH += sys/peripheral_driver/CMSIS/Device/ST/STM32L1xx/Source/Templates

# C source files
SOURCES += ./sys/peripheral_driver/CMSIS/Device/ST/STM32L1xx/Source/Templates/system_stm32l1xx.c