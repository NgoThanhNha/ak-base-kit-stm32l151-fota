CFLAGS 		+= -I./networks/ethernet/socket
CPPFLAGS 	+= -I./networks/ethernet/socket

VPATH += ./networks/ethernet/socket

#C source files
SOURCES += ./networks/ethernet/socket/socket.c