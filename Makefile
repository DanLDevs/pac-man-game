
# Detect OS
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
	CXX = g++
	EXEC = pac-man.exe
	CXXFLAGS = -std=c++11 -I ./includes
	LDFLAGS = -static -lwinmm -lgdi32
	LIBS = lib/libraylib.dll.a
else ifeq ($(UNAME_S),Linux)
	CXX = clang++
	EXEC = pac-man
	CXXFLAGS = -std=c++11 -I ./includes
	LDFLAGS = -lm -lpthread -ldl -lrt -lX11
	LIBS = lib/libraylib_linux.a
else ifeq ($(UNAME_S),Darwin)
	CXX = clang++
	EXEC = pac-man
	CXXFLAGS = -std=c++11 -I ./includes
	LDFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
	LIBS = lib/libraylib_macos.a
endif

SRC = \
	src/main.cpp \
	src/game.cpp \
	src/utils.cpp \
	src/pixels.cpp \
	src/Map/Map.cpp \
	src/Key/Key.cpp \
	src/PacMan/PacMan.cpp \
	src/PacMan/Cpu.cpp

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS = -lm -lpthread -ldl -lrt -lX11
	LIBS = lib/libraylib_linux.a
else ifeq ($(UNAME_S),Darwin)
	LDFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
	LIBS = lib/libraylib_macos.a
endif


all: $(EXEC)

$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LIBS) -o $(EXEC) $(LDFLAGS)

clean:
	@if [ -f $(EXEC) ]; then rm -f $(EXEC); fi


re: clean all

run: all
	./$(EXEC)

.PHONY: all clean run
