import os
import sys
import shutil
import time
import re

executable_name = "exec"

makefile_string = "CC= g++\nSOURCES= $(wildcard *.cpp)\nHEADERS= $(wildcard *.h)\nOBJECTS= $(SOURCES:.cpp=.o)\nEXECUTABLE= exec\n.PHONY: all debug\n\nall: prep-all\n	@echo Make all has finished\n\nprep-all: CFLAGS = -Wall -Werror -Wextra -pedantic -Wshadow -Wfatal-errors -lcurl -ljsoncpp\nprep-all: $(EXECUTABLE)\n\ndebug: prep-debug\n	@echo Make debug has finished\n\nprep-debug: CFLAGS = -g -Wall -Werror -Wextra -pedantic -Wshadow -Wfatal-errors -lcurl -ljsoncpp\nprep-debug: $(EXECUTABLE)\n\n$(EXECUTABLE): $(OBJECTS)\n	$(CC) $(OBJECTS) -lcurl -ljsoncpp -o $@\n\n%.o: %.cpp $(HEADERS)\n	$(CC) $(CFLAGS) -c -o $@ $<\n"


def var_is_executable(item):
    if os.path.isfile(item) and (item.endswith('.bin') or item.endswith('.exe') or item.endswith(executable_name)):
        return True

#This sucked literal ass since walk() genuinely doesn't work properly with subdirectories within subdirectories
def copy_header_source_file(file):
    if file.endswith('.h') or file.endswith('.cpp'):
        #print("COPYING FILE:"+file)
        shutil.copy2(file,'./construction_yard')

def recursive_search_dir(path):
    listing = os.listdir(path)
    for item in listing:
        #print("THE CURRENT ITEM IS:"+item)
        if os.path.isfile(item) or item.find('.') != -1:
            copy_header_source_file(os.path.join(path, item))
        else:
            if item.find("git") == -1 and item.find("vscode") == -1 and item.find("saved_info") == -1 and item.find("construction_yard") == -1:
                new_path = os.path.join(path, item)
                recursive_search_dir(new_path)


def delete_existing_executable():
    listing = os.listdir(".")
    for item in listing:
        if var_is_executable(item):
            os.remove(item)


def delete_construction_yard_sources():
    if not os.path.exists("construction_yard"):
        os.mkdir("construction_yard")
    os.chdir("construction_yard")
    listing = os.listdir(".")
    for item in listing:
        #print("THE CURRENT ITEM IS:"+item)
        if os.path.isfile(item) and (item.endswith('.h') or item.endswith('.cpp') or item.endswith('.o')):
            #print("ATTEMPTING TO DELETE ITEM: "+item)
            os.remove(item)
    os.chdir("../")

def copy_exec_from_construction_yard():
    listing = os.listdir("construction_yard")
    for item in listing:
        #print("FOUND ITEM")
        #print(item)
        path_to_item = os.path.join("./construction_yard/", item)
        if var_is_executable(path_to_item):
            #print("COPYING ITEM")
            #print(path_to_item)
            shutil.copy(path_to_item, os.getcwd())

def create_makefile():
    #Assumes we are in construction_yard
    text_file = open("Makefile","w")
    text_file.write(makefile_string)
    text_file.close()

def preprocessor():
    #Preprocessor
    os.chdir("construction_yard")
    listing = os.listdir(".")
    preproc_map = {}
    for item in listing:
        if os.path.isfile(item) and (item.endswith('.h') or item.endswith('.cpp') or item.endswith('.o')):
            #On first pass we read in all #LAMBDA_CONT and clear those lines
            file = open(item, "r+")
            file_string = file.read()
            #print("FILETYPYE:", type(file_string))
            #print("WORKING WITH FILE_STRING:" + file_string)
            file.truncate(0)#Deletes existing contents of file, will replace with file_string that has erased all #LAMBDA_CONT
            file.seek(0)
            #We have the entire file as a string now
            lambda_conts = re.findall("#LAMBDA_CONT[ \t]*\`([ \t]*\w+[ \t]*`[ \t]*.+[ \t]*)\`", file_string)
            #lamda_conts_quotes = re.findall("#LAMBDA_CONT[ \t]*\`([ \t]*\w+[ \t]*`[ \t]*\"[ \t]*\w+[ \t]*\"[ \t]*)\`", file_string)
            #print("FOUND LAMBDA_CONTS:", lambda_conts)
            file_string = re.sub("#LAMBDA_CONT[ \t]*\`([ \t]*\w+[ \t]*`[ \t]*.+[ \t]*)\`", "", file_string)#Erase #LAMBDA_CONTs
            #print("FILE STRING WITHOUT LAMBDA_CONT:" + file_string)
            #Paste file_string back into the file
            file.write(file_string)
            file.close()
            #Divide and integrate lambda_conts into preproc_map
            for word_def in lambda_conts:
                word, definition = re.split("`", word_def)
                if word in preproc_map:
                    preproc_map[word] += "\n" + definition
                else:
                    preproc_map[word] = definition
    #print("WORKING WITH PREPROC_MAP:", preproc_map)

    for item in listing:
        if os.path.isfile(item) and (item.endswith('.h') or item.endswith('.cpp') or item.endswith('.o')):
            #On second pass we read in and replace all #LAMBDA_START
            file = open(item, "r+")
            file_string = file.read()
            file.truncate(0)#Deletes existing contents of file
            file.seek(0)
            for entry in preproc_map:
                file_string = re.sub("#LAMBDA_START[ \t]*\`[ \t]*" + entry + "[ \t]*\`", preproc_map[entry], file_string)
            file.write(file_string)
            #print("FILE STRING WITHOUT LAMBDA_START:" + file_string)
            file.close()
    os.chdir("../")
    #End of preprocessor

def copy_sources_build_exec(make_arg):
    #Delete all source files in the construction_yard
    delete_construction_yard_sources()
    #Copy all source files to the construction_yard
    recursive_search_dir(".")
    #Run the makefile present in the construction yard with the requested arg
    delete_existing_executable()#delete exe in main folder
    preprocessor()
    os.chdir("construction_yard")
    if not os.path.isfile("Makefile"):
        create_makefile()
    delete_existing_executable()#delete exe in construction_yard
    make_string = "make " + make_arg
    print("Calling makefile with: " + make_string)
    os.system(make_string)
    os.chdir("../")
    copy_exec_from_construction_yard()
    os.chdir("./construction_yard")
    delete_existing_executable()
    os.chdir("../")




#MAIN
#Check if our number of args is valid
if (len(sys.argv) != 2):
    print("Please enter exactly one argument with this script: <all>, <debug_all> are valid. Alternatively <build_*> can be used in conjunction to not launch the program.")
    quit()

launch_program = False
launch_gdb = False
make_arg = sys.argv[1]

match make_arg:
    case "build_debug":
        make_arg = "debug"
    case "debug":
        launch_gdb = True
    case "build_all":
        make_arg = "all"
    case "all":
        launch_program = True

#Set debug_mode 1 or 0
if not os.path.exists("auto_generated"):
    os.mkdir("auto_generated")
os.chdir("auto_generated")
with open("debug_mode.h", "w") as file:
    file.write("#ifndef DEBUG_MODE_H\n")
    file.write("#define DEBUG_MODE_H\n")
    if (make_arg == "debug"):
        file.write("#define DEBUG 1\n")
    else:
        file.write("#define DEBUG 0\n")
    file.write("#endif\n")
os.chdir("../")

        
copy_sources_build_exec(make_arg)

if (launch_program or launch_gdb):
    #Find and launch exec
    listing = os.listdir(".")
    exec = "unknown"
    for item in listing:
        if var_is_executable(item):
            exec = item

    if (not launch_gdb):
        #Delete sources before program runs unless we are going to go into gdb, in which case keep the sources around until after we close gdb(needed to more cleanly investigate source code in gdb)
        delete_construction_yard_sources()
    if (exec == "unknown"):
        print("Failed to find executable, clearing construction yard sources and exiting\n")
        launch_gdb = launch_program = False
    if (launch_program):
        print("Launching program: " + exec + "\n")
        os.system("./" + exec)
    if (launch_gdb):
        print("Launching program under gdb: " + exec + "\n")
        os.system("gdb -x misc/gdb_initial_commands.txt " + exec)

delete_construction_yard_sources()