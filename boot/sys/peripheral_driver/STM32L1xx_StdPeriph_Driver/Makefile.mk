CFLAGS += -I./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/inc
CPPFLAGS += -I./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/inc

VPATH += sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src

# C source files
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_gpio.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_rcc.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_usart.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_spi.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/misc.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_exti.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_syscfg.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_tim.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_adc.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_rtc.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_pwr.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_flash.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_flash_ramfunc.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_iwdg.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_comp.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_dac.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_i2c.c
SOURCES += ./sys/peripheral_driver/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_dma.c