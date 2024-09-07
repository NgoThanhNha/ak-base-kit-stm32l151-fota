include common/crc32/Makefile.mk

CFLAGS += -I./common
CPPFLAGS += -I./common

VPATH += ./common

# C source files
SOURCES += ./common/xprintf.c