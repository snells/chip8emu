
files = main.c op.c
cc = g++
compiler_flags = -w
linker_flags = -lSDL2
exe = chip8

all : $(files)
	$(cc) $(files) $(compiler_flags) $(linker_flags) -o $(exe)
