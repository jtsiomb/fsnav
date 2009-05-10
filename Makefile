PREFIX = /usr/local

csrc = $(wildcard src/*.c src/vmath/*.c src/image/*.c)
ccsrc = $(wildcard src/*.cc src/vmath/*.cc src/image/*.cc)
obj = $(ccsrc:.cc=.o) $(csrc:.c=.o)
bin = fsnav

inc = -Isrc -Isrc/vmath -Isrc/image -I/usr/local/include

ifeq ($(shell uname -s), CYGWIN_NT-5.1)
	inc += -I/usr/include/opengl
endif

CC = gcc
CXX = g++
CFLAGS = -pedantic -Wall -g -O3 $(inc) $(def) -DPREFIX=\"$(PREFIX)\" `pkg-config --cflags freetype2`
CXXFLAGS = -pedantic -Wall -g -O3 $(inc) $(def) -DPREFIX=\"$(PREFIX)\"
LDFLAGS = $(libgl_$(shell uname -s)) `pkg-config --libs freetype2` -lpng -ljpeg -lm

libgl_UNIX = -lGL -lGLU -lglut
libgl_Linux = $(libgl_UNIX)
libgl_IRIX = $(libgl_UNIX)
libgl_FreeBSD = $(libgl_UNIX)
libgl_Darwin = -framework OpenGL -framework GLUT
libgl_CYGWIN_NT-5.1 = -lopengl32 -lglu32 -lglut32

$(bin): $(obj)
	$(CXX) -o $@ $(obj) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: install
install:
	install -m 755 $(bin) $(PREFIX)/bin/$(bin)
	install -d $(PREFIX)/share/fsnav
	install -m 644 data/* $(PREFIX)/share/fsnav/

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/bin/$(bin)
	rm -f $(PREFIX)/share/fsnav/*
	rmdir $(PREFIX)/share/fsnav
