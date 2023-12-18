import os
import sys
import shutil

#This sucked literal ass since walk() genuinely doesn't work properly with subdirectories within subdirectories
def copy_file(file):
    if file.endswith('.h') or file.endswith('.cpp'):
        #print("COPYING FILE:"+file)
        shutil.copy2(file,'./construction_yard')  

def recursive_search_dir(path):
    listing = os.listdir(path)
    for item in listing:
        #print("THE CURRENT ITEM IS:"+item)
        if os.path.isfile(item) or item.find('.') != -1:
            copy_file(os.path.join(path, item))
        else:
            if item.find("git") == -1 and item.find("vscode") == -1 and item.find("saved_info") == -1 and item.find("construction_yard") == -1:
                new_path = os.path.join(path, item)
                recursive_search_dir(new_path)

def delete_construction_yard_sources():
    listing = os.listdir(".")
    for item in listing:
        #print("THE CURRENT ITEM IS:"+item)
        if os.path.isfile(item) and (item.endswith('.h') or item.endswith('.cpp')):
            #print("ATTEMPTING TO DELETE ITEM: "+item)
            os.remove(item)


#MAIN
#Check if our number of args is valid
if (len(sys.argv) != 2):
    print("Please enter exactly one argument with this script: <all> or <clean> are valid")
    quit()
#Copy all source files to the construction_yard
recursive_search_dir(".")
#Run the makefile present in the construction yard with the requested arg
os.chdir("construction_yard")
make_string = "make " + sys.argv[1]
print("Calling makefile with: " + make_string)
os.system(make_string)
#Delete all source files in the construction_yard
delete_construction_yard_sources()