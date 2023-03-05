CXX := g++
CXXFLAGS := -Wall -std=c++11
target = chip8
objects = main.o chip8.o

$(target) : $(objects)
	$(CXX) -o $(target) $(objects) lib/libglfw.3.dylib lib/libglad.a

main.o : GLFW/glfw3.h glad/gl.h chip8.h
chip8.o : chip8.h

.PHONY: clean
clean:
	$(RM) $(target) $(objects)