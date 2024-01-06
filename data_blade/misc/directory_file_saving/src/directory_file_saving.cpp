#include <filesystem>
#include <iostream>
#include <fstream>
#include "directory_file_saving.h"

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

bool check_and_create_file(std::fstream * file_handle, std::ios_base::openmode flags, std::string file_path_string) {

    if (!std::filesystem::exists(file_path_string)) {
        flags |= std::ios::trunc;
    }

    file_handle->open(file_path_string, flags);

    if (*file_handle) {
        return true;
    }

    if (file_handle->is_open()) {
        file_handle->close();
    }

    return false;
}