# build:
# 	g++ -Wfatal-errors -g \
# 	-std=c++14 \
# 	./*.cpp \
# 	-I ./include \
# 	-L ./lib \
# 	-lmingw32 \
# 	-lSDL2main \
# 	-lSDL2 \
# 	-o screen.exe

INCLUDES= -I ./include
FLAGS= -g

OBJECTS= main.cpp ./include/chip8memory.cpp ./include/chip8stack.cpp ./include/chip8keyboard.cpp ./include/chip8.cpp ./include/chip8screen.cpp
all: ${OBJECTS}
	g++ ${FLAGS} ${INCLUDES} ${OBJECTS} -L ./lib -lmingw32 -lSDL2main -lSDL2 -o screen

clean:
	del screen.exe
