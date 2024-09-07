include sys/peripheral_driver/Makefile.mk

CFLAGS 		+= -I./sys
CPPFLAGS 	+= -I./sys

VPATH += ./sys

LDFILE = sys/stk.ld 

#C source files
SOURCES += ./sys/io_cfg.c
SOURCES += ./sys/sys_cfg.c
SOURCES += ./sys/sys_irq.c
SOURCES += ./sys/sys_boot.c
SOURCES += ./sys/sys_startup.c