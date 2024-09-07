include networks/cloud/MQTT/MQTTPacket/src/Makefile.mk

CFLAGS 		+= -I./networks/cloud/MQTT
CPPFLAGS 	+= -I./networks/cloud/MQTT

VPATH += ./networks/cloud/MQTT

#C source files
SOURCES += ./networks/cloud/MQTT/mqtt_port.c
SOURCES += ./networks/cloud/MQTT/MQTTClient.c