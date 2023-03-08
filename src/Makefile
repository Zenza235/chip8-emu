CXX      := g++
CXXFLAGS := -Wall -std=c++11
target    = chip8
objects   = main.o chip8.o
lib       = lib/$(windows)
mac       = libglfw.3.dylib
windows   = windows/libglfw3dll.a

$(target) : $(objects)
	$(CXX) -o $(target) $(objects) lib/libglad.a

main.o : GLFW/glfw3.h glad/gl.h chip8.h
chip8.o : chip8.h

.PHONY: clean
clean:
	$(RM) $(target) $(objects)