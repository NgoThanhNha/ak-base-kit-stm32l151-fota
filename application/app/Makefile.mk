CFLAGS 		+= -I./app
CPPFLAGS 	+= -I./app

VPATH += ./app

#CPP source files
SOURCES_CPP += ./app/app.cpp
SOURCES_CPP += ./app/bsp.cpp
SOURCES_CPP += ./app/shell.cpp
SOURCES_CPP += ./app/task_list.cpp
SOURCES_CPP += ./app/task_life.cpp
SOURCES_CPP += ./app/task_shell.cpp
SOURCES_CPP += ./app/task_dbg.cpp
SOURCES_CPP += ./app/task_cloud.cpp
SOURCES_CPP += ./app/task_fw.cpp