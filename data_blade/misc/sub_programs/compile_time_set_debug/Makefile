ifeq ($(OS),Windows_NT)
	RM = del /Q /F
ifdef ComSpec
	SHELL := $(ComSpec)
endif
ifdef COMSPEC
	SHELL := $(COMSPEC)
endif
else
	RM = rm -f
endif


all: purge_all make_all clean_all

make_all: compile_time_set_debug_make

clean_all: 
	$(RM) *.o

purge_all: 
	$(RM) *.exe *.o

compile_time_set_debug_make: compile_time_set_debug.o
	g++ -g -o compile_time_set_debug compile_time_set_debug.o

compile_time_set_debug.o: compile_time_set_debug.cpp
	g++ -c -g compile_time_set_debug.cpp

