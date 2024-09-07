CFLAGS 		+= -I./networks/cloud/MQTT/MQTTPacket/src
CPPFLAGS 	+= -I./networks/cloud/MQTT/MQTTPacket/src

VPATH += ./networks/cloud/MQTT/MQTTPacket/src

#C source files
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTConnectServer.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTConnectClient.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTDeserializePublish.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTFormat.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTPacket.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTSerializePublish.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTSubscribeServer.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTSubscribeClient.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.c
SOURCES += ./networks/cloud/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.c