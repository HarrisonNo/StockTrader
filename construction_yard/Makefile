CC= g++
CFLAGS= -Wall -Werror -Wextra -pedantic -Wshadow -Wfatal-errors
SOURCES= $(wildcard *.cpp)
HEADERS= $(wildcard *.h)
OBJECTS= $(SOURCES:.cpp=.o)
EXECUTABLE= main
#Stealing this layout from https://stackoverflow.com/questions/37952230/basic-makefile-with-two-cpp-files-and-a-header-file LMFAO this is so inefficient, maybe I'll clean this up at some point. Maybe

all: $(EXECUTABLE)
	@echo Make has finished

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *~ $(EXECUTABLE)