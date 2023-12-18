CC= g++
CFLAGS= -Wall -Werror -Wextra -pedantic -Wshadow -Wfatal-errors
SOURCES= public_logical_account.cpp private_logical_account.cpp public_logical_ticker.cpp private_logical_ticker.cpp main.cpp unit_tests.cpp debug_api.cpp debug_func_replacements.cpp broker_api_class.cpp directory_file_saving.cpp
HEADERS= unit_tests.h logical_account.h logical_ticker.h global_defines.h directory_file_saving.h debug_func_replacements.h api.h debug_api.h assert_and_verify.h broker_api_class.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE= main
#Stealing this layout from https://stackoverflow.com/questions/37952230/basic-makefile-with-two-cpp-files-and-a-header-file LMFAO this is so inefficient, maybe I'll clean this up at some point. Maybe

all: $(EXECUTABLE)
	@echo Make has finished

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *~ $(EXECUTABLE)