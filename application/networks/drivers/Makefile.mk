CFLAGS 		+= -I./networks/drivers
CPPFLAGS 	+= -I./networks/drivers

VPATH += ./networks/drivers

#C source files
SOURCES += ./networks/drivers/w5500.c