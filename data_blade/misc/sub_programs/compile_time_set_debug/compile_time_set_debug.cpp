//This program changes the values of FULL_DEBUG, DEBUG, and VERBOSE
//Should be used like so:
// ./compile_time_set_debug 0 0 0
// With the ordering being FULL_DEBUG PARTIAL_DEBUG VERBOSE
#include <fstream>
#include <iostream>
#define ALL_FILE_PATH "../asserts_and_verifies"
#define OPEN_IN_OUT_TRUNC std::ios::in | std::ios::out | std::ios::trunc
int main(int argc, char** argv){
    std::ofstream thefile;
    bool enabled_full_debug = false;
    //argv[0] = "./compile_time_set_debug" --name of program here
    //argv[1] = 0 --value of FULL_DEBUG
    //argv[2] = 0 --value of PARTIAL_DEBUG
    //argv[3] = 0 --value of VERBOSE
    if (argc != 4){
        std::cout<<"Invalid number of args\n";
        return -1;//Invalid number of args
    }

    if (argv[1][0] == '1') {//FULL_DEBUG is enabled
        thefile.open(ALL_FILE_PATH"/FULL_DEBUG.h", OPEN_IN_OUT_TRUNC);
        thefile<<"#define FULL_DEBUG 1\n";
        thefile.close();
        enabled_full_debug = true;
    } else {//FULL_DEBUG is not enabled
        thefile.open(ALL_FILE_PATH"/FULL_DEBUG.h", OPEN_IN_OUT_TRUNC);
        thefile<<"#define FULL_DEBUG 0\n";
        thefile.close();
    }

    if (!enabled_full_debug && argv[2][0] == '1'){//Enable PARTIAL_DEBUG only if we haven't already set FULL_DEBUG
        thefile.open(ALL_FILE_PATH"/PARTIAL_DEBUG.h", OPEN_IN_OUT_TRUNC);
        thefile<<"#define PARTIAL_DEBUG 1\n";
        thefile.close();
    } else {
        thefile.open(ALL_FILE_PATH"/PARTIAL_DEBUG.h", OPEN_IN_OUT_TRUNC);
        thefile<<"#define PARTIAL_DEBUG 0\n";
        thefile.close();
    }

    if (argv[3][0] == '1') {//VERBOSE is enabled
        thefile.open(ALL_FILE_PATH"/VERBOSE.h", OPEN_IN_OUT_TRUNC);
        thefile<<"#define VERBOSE 1\n";
        thefile.close();
    } else {
        thefile.open(ALL_FILE_PATH"/VERBOSE.h", OPEN_IN_OUT_TRUNC);
        thefile<<"#define VERBOSE 0\n";
        thefile.close();
    }

    return 0;
}