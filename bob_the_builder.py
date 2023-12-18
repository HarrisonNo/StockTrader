import os
import sys
import shutil

#This sucked literal ass since walk() genuinely doesn't work properly with subdirectories within subdirectories
def copy_file(file):
    if file.endswith('.h') or file.endswith('.cpp'):
        print("COPYING FILE:"+file)
        shutil.copy2(file,'./construction_yard')  

def recursive_search_dir(path):
    listing = os.listdir(path)
    for item in listing:
        print("THE CURRENT ITEM IS:"+item)
        if os.path.isfile(item) or item.find('.') != -1:
            copy_file(os.path.join(path, item))
        else:
            if item.find("git") == -1 and item.find("vscode") == -1 and item.find("saved_info") == -1 and item.find("construction_yard") == -1:
                new_path = os.path.join(path, item)
                recursive_search_dir(new_path)



#MAIN
if (len(sys.argv) != 2):
    print("Please enter exactly one argument with this script: <all> or <clean> are valid")
    quit()

#TODO Implement deleting previous files as I have no idea if copy2 overwrites files (not that I even trust the python implementation enough to rely on it)

recursive_search_dir(".")



#os.chdir("construction_yard")
#make_string = "make " + sys.argv[1]
#print("Calling make as:" + make_string)
#os.system(make_string)