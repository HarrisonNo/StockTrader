#include "directory_file_saving.h"
#include <filesystem>
#include <iostream>
/*
https://en.cppreference.com/w/cpp/filesystem
https://cplusplus.com/doc/tutorial/files/
https://en.cppreference.com/w/cpp/filesystem
*/

bool check_and_create_dirs(std::string path) {
    std::error_code ec;
    std::filesystem::path current_path = std::filesystem::current_path(ec);
    //std::cout<<"The RECEIEVED PATH IS "<<path<<std::endl;
    //std::cout<<"THE CURRENT_PATH IS "<<current_path<<std::endl;

    //ec will have some value on fail, zero on success
    if (ec.value()) {
        //std::cout<<"returning with false as current path already exists"<<std::endl;
        return false;
    }

    current_path.append(path);

    //std::cout<<"THE CURRENT_PATH IS NOW "<<current_path<<std::endl;

    if (std::filesystem::exists(current_path)) {
        //std::cout<<"returning with false as NOW current path already exists"<<std::endl;
        return true;
    }

    std::filesystem::create_directories(current_path, ec);

    if (ec.value()) {
        return false;
    }

    return true;
}