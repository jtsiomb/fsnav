csrc = $(wildcard src/*.c)
ccsrc = $(wildcard src/*.cc)
obj = $(ccsrc:.cc=.o) $(csrc:.c=.o)
bin = fsnav

CC = gcc
CXX = g++
CFLAGS = -pedantic -Wall -g -O3 `pkg-config --cflags vmath freetype2`
CXXFLAGS = -pedantic -Wall -g -O3 `pkg-config --cflags vmath`
LDFLAGS = $(libgl_$(shell uname -s)) `pkg-config --libs vmath freetype2` -lm

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
