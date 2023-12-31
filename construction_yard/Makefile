CC= g++
SOURCES= $(wildcard *.cpp)
HEADERS= $(wildcard *.h)
OBJECTS= $(SOURCES:.cpp=.o)
EXECUTABLE= exec
.PHONY: all debug
#Stealing this layout from https://stackoverflow.com/questions/37952230/basic-makefile-with-two-cpp-files-and-a-header-file LMFAO this is so inefficient, maybe I'll clean this up at some point. Maybe

#all resets the cflags to not include -g(which is exclusively used for gdb)
all: prep-all
	@echo Make all has finished

prep-all: CFLAGS = -Wall -Werror -Wextra -pedantic -Wshadow -Wfatal-errors
prep-all: $(EXECUTABLE)

debug: prep-debug
	@echo Make debug has finished

prep-debug: CFLAGS = -g -Wall -Werror -Wextra -pedantic -Wshadow -Wfatal-errors
prep-debug: $(EXECUTABLE)

#all and debug
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<