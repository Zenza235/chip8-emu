CXX      := g++
CXXFLAGS := -Wall -std=c++11
target    = chip8
objects   = main.o chip8.o
lib_paths = $(mac)
mac       = lib/mac/libglfw.3.dylib lib/mac/libglad.a
windows   = lib/windows/libglfw3dll.a # TODO: libglad for windows

$(target) : $(objects)
	$(CXX) -o $(target) $(objects) $(lib_paths)

main.o : GLFW/glfw3.h glad/gl.h chip8.h
chip8.o : chip8.h

.PHONY: clean
clean:
	$(RM) $(target) $(objects)