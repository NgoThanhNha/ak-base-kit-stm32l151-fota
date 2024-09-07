CFLAGS 		+= -I./networks/port
CPPFLAGS 	+= -I./networks/port

VPATH += ./networks/port

#C source files
SOURCES += ./networks/port/w5500_port.c