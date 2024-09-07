CFLAGS 		+= -I./networks/ethernet/DHCP
CPPFLAGS 	+= -I./networks/ethernet/DHCP

VPATH += ./networks/ethernet/DHCP

#C source files
SOURCES += ./networks/ethernet/DHCP/dhcp.c