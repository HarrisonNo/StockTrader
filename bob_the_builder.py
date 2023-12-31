import os
import sys
import shutil

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
        if os.path.isfile(item):
            if item.endswith('.bin') or item.endswith('.exe'):
                os.remove(item)


def delete_construction_yard_sources():
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
        if os.path.isfile(path_to_item):
            #print("FOUND FILE")
            #print(path_to_item)
            if path_to_item.endswith('.bin') or path_to_item.endswith('.exe'):
                #print("COPYING ITEM")
                #print(path_to_item)
                shutil.copy(path_to_item, os.getcwd())

def copy_sources_build_exec(make_arg):
    #Delete all source files in the construction_yard
    delete_construction_yard_sources()
    #Copy all source files to the construction_yard
    recursive_search_dir(".")
    #Run the makefile present in the construction yard with the requested arg
    delete_existing_executable()#delete exe in main folder
    os.chdir("construction_yard")
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
        
copy_sources_build_exec(make_arg)

if (launch_program or launch_gdb):
    #Find and launch exec
    listing = os.listdir(".")
    exec = "unknown"
    for item in listing:
        if os.path.isfile(item):
            if item.endswith('.bin') or item.endswith('.exe'):
                exec = item

    if (not launch_gdb):
        #Delete sources before program runs unless we are going to go into gdb, in which case keep the sources around until after we close gdb(needed to more cleanly investigate source code in gdb)
        delete_construction_yard_sources()
    if (launch_program):
        print("Launching program: " + exec + "\n")
        os.system(exec)
    if (launch_gdb):
        print("Launching program under gdb: " + exec + "\n")
        os.system("gdb " + exec)
        delete_construction_yard_sources()

