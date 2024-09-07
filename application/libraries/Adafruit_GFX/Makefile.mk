CFLAGS 		+= -I./libraries/Adafruit_GFX
CPPFLAGS 	+= -I./libraries/Adafruit_GFX

VPATH += ./libraries/Adafruit_GFX

# CPP source files
SOURCES_CPP += ./libraries/Adafruit_GFX/Adafruit_GFX.cpp
SOURCES_CPP += ./libraries/Adafruit_GFX/glcdfont.cpp
SOURCES_CPP += ./libraries/Adafruit_GFX/Print.cpp