# subdir and objects in current dir
SUBDIRS	= board iniparser scan_com serial spi version front_panel pool net
OBJECTS	= main.o


all:subdirs ${OBJECTS}

	
clean:cleansubdirs
	rm -f ${OBJECTS}
	
include ${MAKEINCLUDE}
